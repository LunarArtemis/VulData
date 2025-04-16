static inline int get_cur_mix_raw(struct usb_mixer_elem_info *cval,
				  int channel, int *value)
{
	return get_ctl_value(cval, UAC_GET_CUR,
			     (cval->control << 8) | channel,
			     value);
}