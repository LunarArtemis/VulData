static int mixer_ctl_connector_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	struct snd_usb_audio *chip = cval->head.mixer->chip;
	int idx = 0, validx, ret, val;

	validx = cval->control << 8 | 0;

	ret = snd_usb_lock_shutdown(chip) ? -EIO : 0;
	if (ret)
		goto error;

	idx = snd_usb_ctrl_intf(chip) | (cval->head.id << 8);
	if (cval->head.mixer->protocol == UAC_VERSION_2) {
		struct uac2_connectors_ctl_blk uac2_conn;

		ret = snd_usb_ctl_msg(chip->dev, usb_rcvctrlpipe(chip->dev, 0), UAC2_CS_CUR,
				      USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
				      validx, idx, &uac2_conn, sizeof(uac2_conn));
		val = !!uac2_conn.bNrChannels;
	} else { /* UAC_VERSION_3 */
		struct uac3_insertion_ctl_blk uac3_conn;

		ret = snd_usb_ctl_msg(chip->dev, usb_rcvctrlpipe(chip->dev, 0), UAC2_CS_CUR,
				      USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
				      validx, idx, &uac3_conn, sizeof(uac3_conn));
		val = !!uac3_conn.bmConInserted;
	}

	snd_usb_unlock_shutdown(chip);

	if (ret < 0) {
error:
		usb_audio_err(chip,
			"cannot get connectors status: req = %#x, wValue = %#x, wIndex = %#x, type = %d\n",
			UAC_GET_CUR, validx, idx, cval->val_type);
		return ret;
	}

	ucontrol->value.integer.value[0] = val;
	return 0;
}