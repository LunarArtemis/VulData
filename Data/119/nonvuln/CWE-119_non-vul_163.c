static int num_med_bfreg(struct mlx5_ib_dev *dev,
			 struct mlx5_bfreg_info *bfregi)
{
	int n;

	n = max_bfregs(dev, bfregi) - bfregi->num_low_latency_bfregs -
	    NUM_NON_BLUE_FLAME_BFREGS;

	return n >= 0 ? n : 0;
}