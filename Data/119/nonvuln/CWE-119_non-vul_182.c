static int mlx5_ib_dct_query_qp(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *mqp,
				struct ib_qp_attr *qp_attr, int qp_attr_mask,
				struct ib_qp_init_attr *qp_init_attr)
{
	struct mlx5_core_dct	*dct = &mqp->dct.mdct;
	u32 *out;
	u32 access_flags = 0;
	int outlen = MLX5_ST_SZ_BYTES(query_dct_out);
	void *dctc;
	int err;
	int supported_mask = IB_QP_STATE |
			     IB_QP_ACCESS_FLAGS |
			     IB_QP_PORT |
			     IB_QP_MIN_RNR_TIMER |
			     IB_QP_AV |
			     IB_QP_PATH_MTU |
			     IB_QP_PKEY_INDEX;

	if (qp_attr_mask & ~supported_mask)
		return -EINVAL;
	if (mqp->state != IB_QPS_RTR)
		return -EINVAL;

	out = kzalloc(outlen, GFP_KERNEL);
	if (!out)
		return -ENOMEM;

	err = mlx5_core_dct_query(dev->mdev, dct, out, outlen);
	if (err)
		goto out;

	dctc = MLX5_ADDR_OF(query_dct_out, out, dct_context_entry);

	if (qp_attr_mask & IB_QP_STATE)
		qp_attr->qp_state = IB_QPS_RTR;

	if (qp_attr_mask & IB_QP_ACCESS_FLAGS) {
		if (MLX5_GET(dctc, dctc, rre))
			access_flags |= IB_ACCESS_REMOTE_READ;
		if (MLX5_GET(dctc, dctc, rwe))
			access_flags |= IB_ACCESS_REMOTE_WRITE;
		if (MLX5_GET(dctc, dctc, rae))
			access_flags |= IB_ACCESS_REMOTE_ATOMIC;
		qp_attr->qp_access_flags = access_flags;
	}

	if (qp_attr_mask & IB_QP_PORT)
		qp_attr->port_num = MLX5_GET(dctc, dctc, port);
	if (qp_attr_mask & IB_QP_MIN_RNR_TIMER)
		qp_attr->min_rnr_timer = MLX5_GET(dctc, dctc, min_rnr_nak);
	if (qp_attr_mask & IB_QP_AV) {
		qp_attr->ah_attr.grh.traffic_class = MLX5_GET(dctc, dctc, tclass);
		qp_attr->ah_attr.grh.flow_label = MLX5_GET(dctc, dctc, flow_label);
		qp_attr->ah_attr.grh.sgid_index = MLX5_GET(dctc, dctc, my_addr_index);
		qp_attr->ah_attr.grh.hop_limit = MLX5_GET(dctc, dctc, hop_limit);
	}
	if (qp_attr_mask & IB_QP_PATH_MTU)
		qp_attr->path_mtu = MLX5_GET(dctc, dctc, mtu);
	if (qp_attr_mask & IB_QP_PKEY_INDEX)
		qp_attr->pkey_index = MLX5_GET(dctc, dctc, pkey_index);
out:
	kfree(out);
	return err;
}