static int set_delay_drop(struct mlx5_ib_dev *dev)
{
	int err = 0;

	mutex_lock(&dev->delay_drop.lock);
	if (dev->delay_drop.activate)
		goto out;

	err = mlx5_core_set_delay_drop(dev->mdev, dev->delay_drop.timeout);
	if (err)
		goto out;

	dev->delay_drop.activate = true;
out:
	mutex_unlock(&dev->delay_drop.lock);

	if (!err)
		atomic_inc(&dev->delay_drop.rqs_cnt);
	return err;
}