static int max_bfregs(struct mlx5_ib_dev *dev, struct mlx5_bfreg_info *bfregi)
{
	return get_num_static_uars(dev, bfregi) * MLX5_NON_FP_BFREGS_PER_UAR;
}