static int uac2_ctl_value_size(int val_type)
{
	switch (val_type) {
	case USB_MIXER_S32:
	case USB_MIXER_U32:
		return 4;
	case USB_MIXER_S16:
	case USB_MIXER_U16:
		return 2;
	default:
		return 1;
	}
	return 0; /* unreachable */
}