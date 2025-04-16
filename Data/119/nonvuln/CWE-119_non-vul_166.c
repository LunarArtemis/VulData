int mlx5_ib_dealloc_xrcd(struct ib_xrcd *xrcd)
{
	struct mlx5_ib_dev *dev = to_mdev(xrcd->device);
	u32 xrcdn = to_mxrcd(xrcd)->xrcdn;
	int err;

	err = mlx5_core_xrcd_dealloc(dev->mdev, xrcdn);
	if (err)
		mlx5_ib_warn(dev, "failed to dealloc xrcdn 0x%x\n", xrcdn);

	kfree(xrcd);
	return 0;
}