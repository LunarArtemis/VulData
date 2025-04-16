static int si4713_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int rval = 0;

	if (!sdev)
		return -ENODEV;

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		rval = si4713_read_property(sdev, SI4713_TX_LINE_INPUT_MUTE,
						&sdev->mute);

		if (rval < 0)
			goto unlock;
	}

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		ctrl->value = get_mute(sdev->mute);
		break;
	}

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}