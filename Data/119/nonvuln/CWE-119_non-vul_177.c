int mlx5_ib_destroy_wq(struct ib_wq *wq)
{
	struct mlx5_ib_dev *dev = to_mdev(wq->device);
	struct mlx5_ib_rwq *rwq = to_mrwq(wq);

	mlx5_core_destroy_rq_tracked(dev->mdev, &rwq->core_qp);
	destroy_user_rq(dev, wq->pd, rwq);
	kfree(rwq);

	return 0;
}