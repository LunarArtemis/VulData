static int si4713_write_econtrol_tune(struct si4713_device *sdev,
				struct v4l2_ext_control *control)
{
	s32 rval = 0;
	u8 power, antcap;

	rval = validate_range(&sdev->sd, control);
	if (rval < 0)
		goto exit;

	mutex_lock(&sdev->mutex);

	switch (control->id) {
	case V4L2_CID_TUNE_POWER_LEVEL:
		power = control->value;
		antcap = sdev->antenna_capacitor;
		break;
	case V4L2_CID_TUNE_ANTENNA_CAPACITOR:
		power = sdev->power_level;
		antcap = control->value;
		break;
	default:
		rval = -EINVAL;
		goto unlock;
	};

	if (sdev->power_state)
		rval = si4713_tx_tune_power(sdev, power, antcap);

	if (rval == 0) {
		sdev->power_level = power;
		sdev->antenna_capacitor = antcap;
	}

unlock:
	mutex_unlock(&sdev->mutex);
exit:
	return rval;
}