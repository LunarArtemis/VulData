long si4713_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	struct si4713_rnl *rnl = arg;
	u16 frequency;
	int rval = 0;

	if (!arg)
		return -EINVAL;

	mutex_lock(&sdev->mutex);
	switch (cmd) {
	case SI4713_IOC_MEASURE_RNL:
		frequency = v4l2_to_si4713(rnl->frequency);

		if (sdev->power_state) {
			/* Set desired measurement frequency */
			rval = si4713_tx_tune_measure(sdev, frequency, 0);
			if (rval < 0)
				goto unlock;
			/* get results from tune status */
			rval = si4713_update_tune_status(sdev);
			if (rval < 0)
				goto unlock;
		}
		rnl->rnl = sdev->tune_rnl;
		break;

	default:
		/* nothing */
		rval = -ENOIOCTLCMD;
	}

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}