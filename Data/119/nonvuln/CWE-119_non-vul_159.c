int mlx5_ib_destroy_rwq_ind_table(struct ib_rwq_ind_table *ib_rwq_ind_tbl)
{
	struct mlx5_ib_rwq_ind_table *rwq_ind_tbl = to_mrwq_ind_table(ib_rwq_ind_tbl);
	struct mlx5_ib_dev *dev = to_mdev(ib_rwq_ind_tbl->device);

	mlx5_core_destroy_rqt(dev->mdev, rwq_ind_tbl->rqtn);

	kfree(rwq_ind_tbl);
	return 0;
}