static void destroy_raw_packet_qp_tir(struct mlx5_ib_dev *dev,
				      struct mlx5_ib_rq *rq)
{
	mlx5_core_destroy_tir(dev->mdev, rq->tirn);
}