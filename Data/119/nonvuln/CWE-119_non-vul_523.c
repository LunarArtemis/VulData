static int create_keep_iface_ctl(struct usb_mixer_interface *mixer)
{
	struct snd_kcontrol *kctl = snd_ctl_new1(&keep_iface_ctl, mixer);

	/* need only one control per card */
	if (snd_ctl_find_id(mixer->chip->card, &kctl->id)) {
		snd_ctl_free_one(kctl);
		return 0;
	}

	return snd_ctl_add(mixer->chip->card, kctl);
}