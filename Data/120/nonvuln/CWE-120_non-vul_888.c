static int si4713_read_econtrol_tune(struct si4713_device *sdev,
				struct v4l2_ext_control *control)
{
	s32 rval = 0;

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		rval = si4713_update_tune_status(sdev);
		if (rval < 0)
			goto unlock;
	}

	switch (control->id) {
	case V4L2_CID_TUNE_POWER_LEVEL:
		control->value = sdev->power_level;
		break;
	case V4L2_CID_TUNE_ANTENNA_CAPACITOR:
		control->value = sdev->antenna_capacitor;
		break;
	default:
		rval = -EINVAL;
	};

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}