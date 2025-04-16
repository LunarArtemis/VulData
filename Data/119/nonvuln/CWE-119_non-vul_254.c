static int mlx5_ib_destroy_dct(struct mlx5_ib_qp *mqp)
{
	struct mlx5_ib_dev *dev = to_mdev(mqp->ibqp.device);

	if (mqp->state == IB_QPS_RTR) {
		int err;

		err = mlx5_core_destroy_dct(dev->mdev, &mqp->dct.mdct);
		if (err) {
			mlx5_ib_warn(dev, "failed to destroy DCT %d\n", err);
			return err;
		}
	}

	kfree(mqp->dct.in);
	kfree(mqp);
	return 0;
}