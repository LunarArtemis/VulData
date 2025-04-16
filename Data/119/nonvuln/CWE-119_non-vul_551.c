static int get_ctl_value(struct usb_mixer_elem_info *cval, int request,
			 int validx, int *value_ret)
{
	validx += cval->idx_off;

	return (cval->head.mixer->protocol == UAC_VERSION_1) ?
		get_ctl_value_v1(cval, request, validx, value_ret) :
		get_ctl_value_v2(cval, request, validx, value_ret);
}