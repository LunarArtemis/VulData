static int si4713_g_modulator(struct v4l2_subdev *sd, struct v4l2_modulator *vm)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int rval = 0;

	if (!sdev) {
		rval = -ENODEV;
		goto exit;
	}

	if (vm->index > 0) {
		rval = -EINVAL;
		goto exit;
	}

	strncpy(vm->name, "FM Modulator", 32);
	vm->capability = V4L2_TUNER_CAP_STEREO | V4L2_TUNER_CAP_LOW |
		V4L2_TUNER_CAP_RDS | V4L2_TUNER_CAP_RDS_CONTROLS;

	/* Report current frequency range limits */
	vm->rangelow = si4713_to_v4l2(FREQ_RANGE_LOW);
	vm->rangehigh = si4713_to_v4l2(FREQ_RANGE_HIGH);

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		u32 comp_en = 0;

		rval = si4713_read_property(sdev, SI4713_TX_COMPONENT_ENABLE,
						&comp_en);
		if (rval < 0)
			goto unlock;

		sdev->stereo = get_status_bit(comp_en, 1, 1 << 1);
		sdev->rds_info.enabled = get_status_bit(comp_en, 2, 1 << 2);
	}

	/* Report current audio mode: mono or stereo */
	if (sdev->stereo)
		vm->txsubchans = V4L2_TUNER_SUB_STEREO;
	else
		vm->txsubchans = V4L2_TUNER_SUB_MONO;

	/* Report rds feature status */
	if (sdev->rds_info.enabled)
		vm->txsubchans |= V4L2_TUNER_SUB_RDS;
	else
		vm->txsubchans &= ~V4L2_TUNER_SUB_RDS;

unlock:
	mutex_unlock(&sdev->mutex);
exit:
	return rval;
}