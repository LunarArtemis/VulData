static int si4713_g_ext_ctrls(struct v4l2_subdev *sd,
				struct v4l2_ext_controls *ctrls)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int i;

	if (ctrls->ctrl_class != V4L2_CTRL_CLASS_FM_TX)
		return -EINVAL;

	for (i = 0; i < ctrls->count; i++) {
		int err;

		switch ((ctrls->controls + i)->id) {
		case V4L2_CID_RDS_TX_PS_NAME:
		case V4L2_CID_RDS_TX_RADIO_TEXT:
			err = si4713_read_econtrol_string(sdev,
							ctrls->controls + i);
			break;
		case V4L2_CID_TUNE_ANTENNA_CAPACITOR:
		case V4L2_CID_TUNE_POWER_LEVEL:
			err = si4713_read_econtrol_tune(sdev,
							ctrls->controls + i);
			break;
		default:
			err = si4713_read_econtrol_integers(sdev,
							ctrls->controls + i);
		}

		if (err < 0) {
			ctrls->error_idx = i;
			return err;
		}
	}

	return 0;
}