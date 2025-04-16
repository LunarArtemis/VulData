static void to_rdma_ah_attr(struct mlx5_ib_dev *ibdev,
			    struct rdma_ah_attr *ah_attr,
			    struct mlx5_qp_path *path)
{

	memset(ah_attr, 0, sizeof(*ah_attr));

	if (!path->port || path->port > ibdev->num_ports)
		return;

	ah_attr->type = rdma_ah_find_type(&ibdev->ib_dev, path->port);

	rdma_ah_set_port_num(ah_attr, path->port);
	rdma_ah_set_sl(ah_attr, path->dci_cfi_prio_sl & 0xf);

	rdma_ah_set_dlid(ah_attr, be16_to_cpu(path->rlid));
	rdma_ah_set_path_bits(ah_attr, path->grh_mlid & 0x7f);
	rdma_ah_set_static_rate(ah_attr,
				path->static_rate ? path->static_rate - 5 : 0);
	if (path->grh_mlid & (1 << 7)) {
		u32 tc_fl = be32_to_cpu(path->tclass_flowlabel);

		rdma_ah_set_grh(ah_attr, NULL,
				tc_fl & 0xfffff,
				path->mgid_index,
				path->hop_limit,
				(tc_fl >> 20) & 0xff);
		rdma_ah_set_dgid_raw(ah_attr, path->rgid);
	}
}