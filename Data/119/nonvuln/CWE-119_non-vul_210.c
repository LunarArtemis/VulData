int mlx5_ib_modify_wq(struct ib_wq *wq, struct ib_wq_attr *wq_attr,
		      u32 wq_attr_mask, struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(wq->device);
	struct mlx5_ib_rwq *rwq = to_mrwq(wq);
	struct mlx5_ib_modify_wq ucmd = {};
	size_t required_cmd_sz;
	int curr_wq_state;
	int wq_state;
	int inlen;
	int err;
	void *rqc;
	void *in;

	required_cmd_sz = offsetof(typeof(ucmd), reserved) + sizeof(ucmd.reserved);
	if (udata->inlen < required_cmd_sz)
		return -EINVAL;

	if (udata->inlen > sizeof(ucmd) &&
	    !ib_is_udata_cleared(udata, sizeof(ucmd),
				 udata->inlen - sizeof(ucmd)))
		return -EOPNOTSUPP;

	if (ib_copy_from_udata(&ucmd, udata, min(sizeof(ucmd), udata->inlen)))
		return -EFAULT;

	if (ucmd.comp_mask || ucmd.reserved)
		return -EOPNOTSUPP;

	inlen = MLX5_ST_SZ_BYTES(modify_rq_in);
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	rqc = MLX5_ADDR_OF(modify_rq_in, in, ctx);

	curr_wq_state = (wq_attr_mask & IB_WQ_CUR_STATE) ?
		wq_attr->curr_wq_state : wq->state;
	wq_state = (wq_attr_mask & IB_WQ_STATE) ?
		wq_attr->wq_state : curr_wq_state;
	if (curr_wq_state == IB_WQS_ERR)
		curr_wq_state = MLX5_RQC_STATE_ERR;
	if (wq_state == IB_WQS_ERR)
		wq_state = MLX5_RQC_STATE_ERR;
	MLX5_SET(modify_rq_in, in, rq_state, curr_wq_state);
	MLX5_SET(rqc, rqc, state, wq_state);

	if (wq_attr_mask & IB_WQ_FLAGS) {
		if (wq_attr->flags_mask & IB_WQ_FLAGS_CVLAN_STRIPPING) {
			if (!(MLX5_CAP_GEN(dev->mdev, eth_net_offloads) &&
			      MLX5_CAP_ETH(dev->mdev, vlan_cap))) {
				mlx5_ib_dbg(dev, "VLAN offloads are not "
					    "supported\n");
				err = -EOPNOTSUPP;
				goto out;
			}
			MLX5_SET64(modify_rq_in, in, modify_bitmask,
				   MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_VSD);
			MLX5_SET(rqc, rqc, vsd,
				 (wq_attr->flags & IB_WQ_FLAGS_CVLAN_STRIPPING) ? 0 : 1);
		}

		if (wq_attr->flags_mask & IB_WQ_FLAGS_PCI_WRITE_END_PADDING) {
			mlx5_ib_dbg(dev, "Modifying scatter end padding is not supported\n");
			err = -EOPNOTSUPP;
			goto out;
		}
	}

	if (curr_wq_state == IB_WQS_RESET && wq_state == IB_WQS_RDY) {
		if (MLX5_CAP_GEN(dev->mdev, modify_rq_counter_set_id)) {
			MLX5_SET64(modify_rq_in, in, modify_bitmask,
				   MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_RQ_COUNTER_SET_ID);
			MLX5_SET(rqc, rqc, counter_set_id,
				 dev->port->cnts.set_id);
		} else
			pr_info_once("%s: Receive WQ counters are not supported on current FW\n",
				     dev->ib_dev.name);
	}

	err = mlx5_core_modify_rq(dev->mdev, rwq->core_qp.qpn, in, inlen);
	if (!err)
		rwq->ibwq.state = (wq_state == MLX5_RQC_STATE_ERR) ? IB_WQS_ERR : wq_state;

out:
	kvfree(in);
	return err;
}