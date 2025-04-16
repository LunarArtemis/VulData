static int first_hi_bfreg(struct mlx5_ib_dev *dev,
			  struct mlx5_bfreg_info *bfregi)
{
	int med;

	med = num_med_bfreg(dev, bfregi);
	return ++med;
}