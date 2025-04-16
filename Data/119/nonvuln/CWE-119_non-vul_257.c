static int mlx5_set_path(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
			 const struct rdma_ah_attr *ah,
			 struct mlx5_qp_path *path, u8 port, int attr_mask,
			 u32 path_flags, const struct ib_qp_attr *attr,
			 bool alt)
{
	const struct ib_global_route *grh = rdma_ah_read_grh(ah);
	int err;
	enum ib_gid_type gid_type;
	u8 ah_flags = rdma_ah_get_ah_flags(ah);
	u8 sl = rdma_ah_get_sl(ah);

	if (attr_mask & IB_QP_PKEY_INDEX)
		path->pkey_index = cpu_to_be16(alt ? attr->alt_pkey_index :
						     attr->pkey_index);

	if (ah_flags & IB_AH_GRH) {
		if (grh->sgid_index >=
		    dev->mdev->port_caps[port - 1].gid_table_len) {
			pr_err("sgid_index (%u) too large. max is %d\n",
			       grh->sgid_index,
			       dev->mdev->port_caps[port - 1].gid_table_len);
			return -EINVAL;
		}
	}

	if (ah->type == RDMA_AH_ATTR_TYPE_ROCE) {
		if (!(ah_flags & IB_AH_GRH))
			return -EINVAL;

		memcpy(path->rmac, ah->roce.dmac, sizeof(ah->roce.dmac));
		if (qp->ibqp.qp_type == IB_QPT_RC ||
		    qp->ibqp.qp_type == IB_QPT_UC ||
		    qp->ibqp.qp_type == IB_QPT_XRC_INI ||
		    qp->ibqp.qp_type == IB_QPT_XRC_TGT)
			path->udp_sport =
				mlx5_get_roce_udp_sport(dev, ah->grh.sgid_attr);
		path->dci_cfi_prio_sl = (sl & 0x7) << 4;
		gid_type = ah->grh.sgid_attr->gid_type;
		if (gid_type == IB_GID_TYPE_ROCE_UDP_ENCAP)
			path->ecn_dscp = (grh->traffic_class >> 2) & 0x3f;
	} else {
		path->fl_free_ar = (path_flags & MLX5_PATH_FLAG_FL) ? 0x80 : 0;
		path->fl_free_ar |=
			(path_flags & MLX5_PATH_FLAG_FREE_AR) ? 0x40 : 0;
		path->rlid = cpu_to_be16(rdma_ah_get_dlid(ah));
		path->grh_mlid = rdma_ah_get_path_bits(ah) & 0x7f;
		if (ah_flags & IB_AH_GRH)
			path->grh_mlid	|= 1 << 7;
		path->dci_cfi_prio_sl = sl & 0xf;
	}

	if (ah_flags & IB_AH_GRH) {
		path->mgid_index = grh->sgid_index;
		path->hop_limit  = grh->hop_limit;
		path->tclass_flowlabel =
			cpu_to_be32((grh->traffic_class << 20) |
				    (grh->flow_label));
		memcpy(path->rgid, grh->dgid.raw, 16);
	}

	err = ib_rate_to_mlx5(dev, rdma_ah_get_static_rate(ah));
	if (err < 0)
		return err;
	path->static_rate = err;
	path->port = port;

	if (attr_mask & IB_QP_TIMEOUT)
		path->ackto_lt = (alt ? attr->alt_timeout : attr->timeout) << 3;

	if ((qp->ibqp.qp_type == IB_QPT_RAW_PACKET) && qp->sq.wqe_cnt)
		return modify_raw_packet_eth_prio(dev->mdev,
						  &qp->raw_packet_qp.sq,
						  sl & 0xf);

	return 0;
}