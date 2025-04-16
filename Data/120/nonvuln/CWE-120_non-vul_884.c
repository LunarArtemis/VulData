static int si4713_read_econtrol_string(struct si4713_device *sdev,
				struct v4l2_ext_control *control)
{
	s32 rval = 0;

	switch (control->id) {
	case V4L2_CID_RDS_TX_PS_NAME:
		if (strlen(sdev->rds_info.ps_name) + 1 > control->size) {
			control->size = MAX_RDS_PS_NAME + 1;
			rval = -ENOSPC;
			goto exit;
		}
		rval = copy_to_user(control->string, sdev->rds_info.ps_name,
					strlen(sdev->rds_info.ps_name) + 1);
		if (rval)
			rval = -EFAULT;
		break;

	case V4L2_CID_RDS_TX_RADIO_TEXT:
		if (strlen(sdev->rds_info.radio_text) + 1 > control->size) {
			control->size = MAX_RDS_RADIO_TEXT + 1;
			rval = -ENOSPC;
			goto exit;
		}
		rval = copy_to_user(control->string, sdev->rds_info.radio_text,
					strlen(sdev->rds_info.radio_text) + 1);
		if (rval)
			rval = -EFAULT;
		break;

	default:
		rval = -EINVAL;
		break;
	};

exit:
	return rval;
}