static int set_mlx_qp_type(struct mlx5_ib_dev *dev,
			   struct ib_qp_init_attr *init_attr,
			   struct mlx5_ib_create_qp *ucmd,
			   struct ib_udata *udata)
{
	enum { MLX_QP_FLAGS = MLX5_QP_FLAG_TYPE_DCT | MLX5_QP_FLAG_TYPE_DCI };
	int err;

	if (!udata)
		return -EINVAL;

	if (udata->inlen < sizeof(*ucmd)) {
		mlx5_ib_dbg(dev, "create_qp user command is smaller than expected\n");
		return -EINVAL;
	}
	err = ib_copy_from_udata(ucmd, udata, sizeof(*ucmd));
	if (err)
		return err;

	if ((ucmd->flags & MLX_QP_FLAGS) == MLX5_QP_FLAG_TYPE_DCI) {
		init_attr->qp_type = MLX5_IB_QPT_DCI;
	} else {
		if ((ucmd->flags & MLX_QP_FLAGS) == MLX5_QP_FLAG_TYPE_DCT) {
			init_attr->qp_type = MLX5_IB_QPT_DCT;
		} else {
			mlx5_ib_dbg(dev, "Invalid QP flags\n");
			return -EINVAL;
		}
	}

	if (!MLX5_CAP_GEN(dev->mdev, dct)) {
		mlx5_ib_dbg(dev, "DC transport is not supported\n");
		return -EOPNOTSUPP;
	}

	return 0;
}