static int mlx5_ib_modify_dct(struct ib_qp *ibqp, struct ib_qp_attr *attr,
			      int attr_mask, struct ib_udata *udata)
{
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	enum ib_qp_state cur_state, new_state;
	int err = 0;
	int required = IB_QP_STATE;
	void *dctc;

	if (!(attr_mask & IB_QP_STATE))
		return -EINVAL;

	cur_state = qp->state;
	new_state = attr->qp_state;

	dctc = MLX5_ADDR_OF(create_dct_in, qp->dct.in, dct_context_entry);
	if (cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT) {
		required |= IB_QP_ACCESS_FLAGS | IB_QP_PKEY_INDEX | IB_QP_PORT;
		if (!is_valid_mask(attr_mask, required, 0))
			return -EINVAL;

		if (attr->port_num == 0 ||
		    attr->port_num > MLX5_CAP_GEN(dev->mdev, num_ports)) {
			mlx5_ib_dbg(dev, "invalid port number %d. number of ports is %d\n",
				    attr->port_num, dev->num_ports);
			return -EINVAL;
		}
		if (attr->qp_access_flags & IB_ACCESS_REMOTE_READ)
			MLX5_SET(dctc, dctc, rre, 1);
		if (attr->qp_access_flags & IB_ACCESS_REMOTE_WRITE)
			MLX5_SET(dctc, dctc, rwe, 1);
		if (attr->qp_access_flags & IB_ACCESS_REMOTE_ATOMIC) {
			if (!mlx5_ib_dc_atomic_is_supported(dev))
				return -EOPNOTSUPP;
			MLX5_SET(dctc, dctc, rae, 1);
			MLX5_SET(dctc, dctc, atomic_mode, MLX5_ATOMIC_MODE_DCT_CX);
		}
		MLX5_SET(dctc, dctc, pkey_index, attr->pkey_index);
		MLX5_SET(dctc, dctc, port, attr->port_num);
		MLX5_SET(dctc, dctc, counter_set_id, dev->port[attr->port_num - 1].cnts.set_id);

	} else if (cur_state == IB_QPS_INIT && new_state == IB_QPS_RTR) {
		struct mlx5_ib_modify_qp_resp resp = {};
		u32 min_resp_len = offsetof(typeof(resp), dctn) +
				   sizeof(resp.dctn);

		if (udata->outlen < min_resp_len)
			return -EINVAL;
		resp.response_length = min_resp_len;

		required |= IB_QP_MIN_RNR_TIMER | IB_QP_AV | IB_QP_PATH_MTU;
		if (!is_valid_mask(attr_mask, required, 0))
			return -EINVAL;
		MLX5_SET(dctc, dctc, min_rnr_nak, attr->min_rnr_timer);
		MLX5_SET(dctc, dctc, tclass, attr->ah_attr.grh.traffic_class);
		MLX5_SET(dctc, dctc, flow_label, attr->ah_attr.grh.flow_label);
		MLX5_SET(dctc, dctc, mtu, attr->path_mtu);
		MLX5_SET(dctc, dctc, my_addr_index, attr->ah_attr.grh.sgid_index);
		MLX5_SET(dctc, dctc, hop_limit, attr->ah_attr.grh.hop_limit);

		err = mlx5_core_create_dct(dev->mdev, &qp->dct.mdct, qp->dct.in,
					   MLX5_ST_SZ_BYTES(create_dct_in));
		if (err)
			return err;
		resp.dctn = qp->dct.mdct.mqp.qpn;
		err = ib_copy_to_udata(udata, &resp, resp.response_length);
		if (err) {
			mlx5_core_destroy_dct(dev->mdev, &qp->dct.mdct);
			return err;
		}
	} else {
		mlx5_ib_warn(dev, "Modify DCT: Invalid transition from %d to %d\n", cur_state, new_state);
		return -EINVAL;
	}
	if (err)
		qp->state = IB_QPS_ERR;
	else
		qp->state = new_state;
	return err;
}