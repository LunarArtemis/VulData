static int snd_usb_mixer_activate(struct usb_mixer_interface *mixer)
{
	int err;

	if (mixer->urb) {
		err = usb_submit_urb(mixer->urb, GFP_NOIO);
		if (err < 0)
			return err;
	}

	return 0;
}