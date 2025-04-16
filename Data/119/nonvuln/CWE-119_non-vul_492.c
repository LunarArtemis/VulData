int snd_usb_create_mixer(struct snd_usb_audio *chip, int ctrlif,
			 int ignore_error)
{
	static struct snd_device_ops dev_ops = {
		.dev_free = snd_usb_mixer_dev_free
	};
	struct usb_mixer_interface *mixer;
	int err;

	strcpy(chip->card->mixername, "USB Mixer");

	mixer = kzalloc(sizeof(*mixer), GFP_KERNEL);
	if (!mixer)
		return -ENOMEM;
	mixer->chip = chip;
	mixer->ignore_ctl_error = ignore_error;
	mixer->id_elems = kcalloc(MAX_ID_ELEMS, sizeof(*mixer->id_elems),
				  GFP_KERNEL);
	if (!mixer->id_elems) {
		kfree(mixer);
		return -ENOMEM;
	}

	mixer->hostif = &usb_ifnum_to_if(chip->dev, ctrlif)->altsetting[0];
	switch (get_iface_desc(mixer->hostif)->bInterfaceProtocol) {
	case UAC_VERSION_1:
	default:
		mixer->protocol = UAC_VERSION_1;
		break;
	case UAC_VERSION_2:
		mixer->protocol = UAC_VERSION_2;
		break;
	case UAC_VERSION_3:
		mixer->protocol = UAC_VERSION_3;
		break;
	}

	if (mixer->protocol == UAC_VERSION_3 &&
			chip->badd_profile >= UAC3_FUNCTION_SUBCLASS_GENERIC_IO) {
		err = snd_usb_mixer_controls_badd(mixer, ctrlif);
		if (err < 0)
			goto _error;
	} else {
		err = snd_usb_mixer_controls(mixer);
		if (err < 0)
			goto _error;
	}

	err = snd_usb_mixer_status_create(mixer);
	if (err < 0)
		goto _error;

	err = create_keep_iface_ctl(mixer);
	if (err < 0)
		goto _error;

	err = snd_usb_mixer_apply_create_quirk(mixer);
	if (err < 0)
		goto _error;

	err = snd_device_new(chip->card, SNDRV_DEV_CODEC, mixer, &dev_ops);
	if (err < 0)
		goto _error;

	if (list_empty(&chip->mixer_list))
		snd_card_ro_proc_new(chip->card, "usbmixer", chip,
				     snd_usb_mixer_proc_read);

	list_add(&mixer->list, &chip->mixer_list);
	return 0;

_error:
	snd_usb_mixer_free(mixer);
	return err;
}