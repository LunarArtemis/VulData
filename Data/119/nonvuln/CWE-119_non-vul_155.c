static int alloc_bfreg(struct mlx5_ib_dev *dev,
		       struct mlx5_bfreg_info *bfregi)
{
	int bfregn = -ENOMEM;

	mutex_lock(&bfregi->lock);
	if (bfregi->ver >= 2) {
		bfregn = alloc_high_class_bfreg(dev, bfregi);
		if (bfregn < 0)
			bfregn = alloc_med_class_bfreg(dev, bfregi);
	}

	if (bfregn < 0) {
		BUILD_BUG_ON(NUM_NON_BLUE_FLAME_BFREGS != 1);
		bfregn = 0;
		bfregi->count[bfregn]++;
	}
	mutex_unlock(&bfregi->lock);

	return bfregn;
}