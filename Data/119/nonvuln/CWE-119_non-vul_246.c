static void destroy_raw_packet_qp_tis(struct mlx5_ib_dev *dev,
				      struct mlx5_ib_sq *sq)
{
	mlx5_core_destroy_tis(dev->mdev, sq->tisn);
}