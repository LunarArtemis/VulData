static int get_relative_value(struct usb_mixer_elem_info *cval, int val)
{
	if (!cval->res)
		cval->res = 1;
	if (val < cval->min)
		return 0;
	else if (val >= cval->max)
		return (cval->max - cval->min + cval->res - 1) / cval->res;
	else
		return (val - cval->min) / cval->res;
}