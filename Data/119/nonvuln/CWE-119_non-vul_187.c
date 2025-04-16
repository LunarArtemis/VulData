static int umr_check_mkey_mask(struct mlx5_ib_dev *dev, u64 mask)
{
	if ((mask & MLX5_MKEY_MASK_PAGE_SIZE &&
	     MLX5_CAP_GEN(dev->mdev, umr_modify_entity_size_disabled)) ||
	    (mask & MLX5_MKEY_MASK_A &&
	     MLX5_CAP_GEN(dev->mdev, umr_modify_atomic_disabled)))
		return -EPERM;
	return 0;
}