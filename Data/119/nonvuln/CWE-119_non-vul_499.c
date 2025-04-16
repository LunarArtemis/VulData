static int get_ctl_value_v1(struct usb_mixer_elem_info *cval, int request,
			    int validx, int *value_ret)
{
	struct snd_usb_audio *chip = cval->head.mixer->chip;
	unsigned char buf[2];
	int val_len = cval->val_type >= USB_MIXER_S16 ? 2 : 1;
	int timeout = 10;
	int idx = 0, err;

	err = snd_usb_lock_shutdown(chip);
	if (err < 0)
		return -EIO;

	while (timeout-- > 0) {
		idx = snd_usb_ctrl_intf(chip) | (cval->head.id << 8);
		err = snd_usb_ctl_msg(chip->dev, usb_rcvctrlpipe(chip->dev, 0), request,
				      USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
				      validx, idx, buf, val_len);
		if (err >= val_len) {
			*value_ret = convert_signed_value(cval, snd_usb_combine_bytes(buf, val_len));
			err = 0;
			goto out;
		} else if (err == -ETIMEDOUT) {
			goto out;
		}
	}
	usb_audio_dbg(chip,
		"cannot get ctl value: req = %#x, wValue = %#x, wIndex = %#x, type = %d\n",
		request, validx, idx, cval->val_type);
	err = -EINVAL;

 out:
	snd_usb_unlock_shutdown(chip);
	return err;
}