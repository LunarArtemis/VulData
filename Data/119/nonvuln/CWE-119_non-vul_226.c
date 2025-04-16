static int first_med_bfreg(struct mlx5_ib_dev *dev,
			   struct mlx5_bfreg_info *bfregi)
{
	return num_med_bfreg(dev, bfregi) ? 1 : -ENOMEM;
}