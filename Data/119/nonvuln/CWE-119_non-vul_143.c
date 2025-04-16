static void destroy_raw_packet_qp_sq(struct mlx5_ib_dev *dev,
				     struct mlx5_ib_sq *sq)
{
	destroy_flow_rule_vport_sq(dev, sq);
	mlx5_core_destroy_sq_tracked(dev->mdev, &sq->base.mqp);
	ib_umem_release(sq->ubuffer.umem);
}