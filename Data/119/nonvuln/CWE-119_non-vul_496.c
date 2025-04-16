static int set_cur_ctl_value(struct usb_mixer_elem_info *cval,
			     int validx, int value)
{
	return snd_usb_mixer_set_ctl_value(cval, UAC_SET_CUR, validx, value);
}