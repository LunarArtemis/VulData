static int alloc_med_class_bfreg(struct mlx5_ib_dev *dev,
				 struct mlx5_bfreg_info *bfregi)
{
	int minidx = first_med_bfreg(dev, bfregi);
	int i;

	if (minidx < 0)
		return minidx;

	for (i = minidx; i < first_hi_bfreg(dev, bfregi); i++) {
		if (bfregi->count[i] < bfregi->count[minidx])
			minidx = i;
		if (!bfregi->count[minidx])
			break;
	}

	bfregi->count[minidx]++;
	return minidx;
}