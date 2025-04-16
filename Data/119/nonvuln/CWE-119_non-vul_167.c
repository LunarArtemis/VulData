static int adjust_bfregn(struct mlx5_ib_dev *dev,
			 struct mlx5_bfreg_info *bfregi, int bfregn)
{
	return bfregn / MLX5_NON_FP_BFREGS_PER_UAR * MLX5_BFREGS_PER_UAR +
				bfregn % MLX5_NON_FP_BFREGS_PER_UAR;
}