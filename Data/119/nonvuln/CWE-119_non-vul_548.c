static int get_ctl_value_v2(struct usb_mixer_elem_info *cval, int request,
			    int validx, int *value_ret)
{
	struct snd_usb_audio *chip = cval->head.mixer->chip;
	/* enough space for one range */
	unsigned char buf[sizeof(__u16) + 3 * sizeof(__u32)];
	unsigned char *val;
	int idx = 0, ret, val_size, size;
	__u8 bRequest;

	val_size = uac2_ctl_value_size(cval->val_type);

	if (request == UAC_GET_CUR) {
		bRequest = UAC2_CS_CUR;
		size = val_size;
	} else {
		bRequest = UAC2_CS_RANGE;
		size = sizeof(__u16) + 3 * val_size;
	}

	memset(buf, 0, sizeof(buf));

	ret = snd_usb_lock_shutdown(chip) ? -EIO : 0;
	if (ret)
		goto error;

	idx = snd_usb_ctrl_intf(chip) | (cval->head.id << 8);
	ret = snd_usb_ctl_msg(chip->dev, usb_rcvctrlpipe(chip->dev, 0), bRequest,
			      USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
			      validx, idx, buf, size);
	snd_usb_unlock_shutdown(chip);

	if (ret < 0) {
error:
		usb_audio_err(chip,
			"cannot get ctl value: req = %#x, wValue = %#x, wIndex = %#x, type = %d\n",
			request, validx, idx, cval->val_type);
		return ret;
	}

	/* FIXME: how should we handle multiple triplets here? */

	switch (request) {
	case UAC_GET_CUR:
		val = buf;
		break;
	case UAC_GET_MIN:
		val = buf + sizeof(__u16);
		break;
	case UAC_GET_MAX:
		val = buf + sizeof(__u16) + val_size;
		break;
	case UAC_GET_RES:
		val = buf + sizeof(__u16) + val_size * 2;
		break;
	default:
		return -EINVAL;
	}

	*value_ret = convert_signed_value(cval,
					  snd_usb_combine_bytes(val, val_size));

	return 0;
}