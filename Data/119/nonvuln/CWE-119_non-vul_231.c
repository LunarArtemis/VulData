int mlx5_ib_destroy_qp(struct ib_qp *qp)
{
	struct mlx5_ib_dev *dev = to_mdev(qp->device);
	struct mlx5_ib_qp *mqp = to_mqp(qp);

	if (unlikely(qp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_destroy_qp(qp);

	if (mqp->qp_sub_type == MLX5_IB_QPT_DCT)
		return mlx5_ib_destroy_dct(mqp);

	destroy_qp_common(dev, mqp);

	kfree(mqp);

	return 0;
}