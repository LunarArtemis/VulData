static int get_abs_value(struct usb_mixer_elem_info *cval, int val)
{
	if (val < 0)
		return cval->min;
	if (!cval->res)
		cval->res = 1;
	val *= cval->res;
	val += cval->min;
	if (val > cval->max)
		return cval->max;
	return val;
}