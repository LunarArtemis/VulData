static int ib_rate_to_mlx5(struct mlx5_ib_dev *dev, u8 rate)
{
	if (rate == IB_RATE_PORT_CURRENT)
		return 0;

	if (rate < IB_RATE_2_5_GBPS || rate > IB_RATE_300_GBPS)
		return -EINVAL;

	while (rate != IB_RATE_PORT_CURRENT &&
	       !(1 << (rate + MLX5_STAT_RATE_OFFSET) &
		 MLX5_CAP_GEN(dev->mdev, stat_rate_support)))
		--rate;

	return rate ? rate + MLX5_STAT_RATE_OFFSET : rate;
}