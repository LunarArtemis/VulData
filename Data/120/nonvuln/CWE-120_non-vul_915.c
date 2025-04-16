static int si4713_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int rval = 0;

	if (!sdev)
		return -ENODEV;

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		if (ctrl->value) {
			rval = si4713_set_mute(sdev, ctrl->value);
			if (rval < 0)
				goto exit;

			rval = si4713_set_power_state(sdev, POWER_DOWN);
		} else {
			rval = si4713_set_power_state(sdev, POWER_UP);
			if (rval < 0)
				goto exit;

			rval = si4713_setup(sdev);
			if (rval < 0)
				goto exit;

			rval = si4713_set_mute(sdev, ctrl->value);
		}
		break;
	}

exit:
	return rval;
}