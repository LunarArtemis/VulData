static int create_raw_packet_qp_tis(struct mlx5_ib_dev *dev,
				    struct mlx5_ib_qp *qp,
				    struct mlx5_ib_sq *sq, u32 tdn)
{
	u32 in[MLX5_ST_SZ_DW(create_tis_in)] = {0};
	void *tisc = MLX5_ADDR_OF(create_tis_in, in, ctx);

	MLX5_SET(tisc, tisc, transport_domain, tdn);
	if (qp->flags & MLX5_IB_QP_UNDERLAY)
		MLX5_SET(tisc, tisc, underlay_qpn, qp->underlay_qpn);

	return mlx5_core_create_tis(dev->mdev, in, sizeof(in), &sq->tisn);
}