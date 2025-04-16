static void destroy_rss_raw_qp_tir(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	mlx5_core_destroy_tir(dev->mdev, qp->rss_qp.tirn);
}