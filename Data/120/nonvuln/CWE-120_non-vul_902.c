static int si4713_s_modulator(struct v4l2_subdev *sd, struct v4l2_modulator *vm)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int rval = 0;
	u16 stereo, rds;
	u32 p;

	if (!sdev)
		return -ENODEV;

	if (vm->index > 0)
		return -EINVAL;

	/* Set audio mode: mono or stereo */
	if (vm->txsubchans & V4L2_TUNER_SUB_STEREO)
		stereo = 1;
	else if (vm->txsubchans & V4L2_TUNER_SUB_MONO)
		stereo = 0;
	else
		return -EINVAL;

	rds = !!(vm->txsubchans & V4L2_TUNER_SUB_RDS);

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		rval = si4713_read_property(sdev,
						SI4713_TX_COMPONENT_ENABLE, &p);
		if (rval < 0)
			goto unlock;

		p = set_bits(p, stereo, 1, 1 << 1);
		p = set_bits(p, rds, 2, 1 << 2);

		rval = si4713_write_property(sdev,
						SI4713_TX_COMPONENT_ENABLE, p);
		if (rval < 0)
			goto unlock;
	}

	sdev->stereo = stereo;
	sdev->rds_info.enabled = rds;

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}