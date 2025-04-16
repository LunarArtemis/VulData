static int get_cur_ctl_value(struct usb_mixer_elem_info *cval,
			     int validx, int *value)
{
	return get_ctl_value(cval, UAC_GET_CUR, validx, value);
}