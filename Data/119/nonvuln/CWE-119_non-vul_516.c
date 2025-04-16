static int convert_signed_value(struct usb_mixer_elem_info *cval, int val)
{
	switch (cval->val_type) {
	case USB_MIXER_BOOLEAN:
		return !!val;
	case USB_MIXER_INV_BOOLEAN:
		return !val;
	case USB_MIXER_U8:
		val &= 0xff;
		break;
	case USB_MIXER_S8:
		val &= 0xff;
		if (val >= 0x80)
			val -= 0x100;
		break;
	case USB_MIXER_U16:
		val &= 0xffff;
		break;
	case USB_MIXER_S16:
		val &= 0xffff;
		if (val >= 0x8000)
			val -= 0x10000;
		break;
	}
	return val;
}