static int get_cluster_channels_v3(struct mixer_build *state, unsigned int cluster_id)
{
	struct uac3_cluster_header_descriptor c_header;
	int err;

	err = snd_usb_ctl_msg(state->chip->dev,
			usb_rcvctrlpipe(state->chip->dev, 0),
			UAC3_CS_REQ_HIGH_CAPABILITY_DESCRIPTOR,
			USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN,
			cluster_id,
			snd_usb_ctrl_intf(state->chip),
			&c_header, sizeof(c_header));
	if (err < 0)
		goto error;
	if (err != sizeof(c_header)) {
		err = -EIO;
		goto error;
	}

	return c_header.bNrChannels;

error:
	usb_audio_err(state->chip, "cannot request logical cluster ID: %d (err: %d)\n", cluster_id, err);
	return err;
}