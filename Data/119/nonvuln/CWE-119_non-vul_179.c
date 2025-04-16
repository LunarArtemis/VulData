static int alloc_high_class_bfreg(struct mlx5_ib_dev *dev,
				  struct mlx5_bfreg_info *bfregi)
{
	int i;

	for (i = first_hi_bfreg(dev, bfregi); i < max_bfregs(dev, bfregi); i++) {
		if (!bfregi->count[i]) {
			bfregi->count[i]++;
			return i;
		}
	}

	return -ENOMEM;
}