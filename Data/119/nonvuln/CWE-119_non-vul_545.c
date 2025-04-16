static int keep_iface_ctl_put(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_interface *mixer = snd_kcontrol_chip(kcontrol);
	bool keep_iface = !!ucontrol->value.integer.value[0];

	if (mixer->chip->keep_iface == keep_iface)
		return 0;
	mixer->chip->keep_iface = keep_iface;
	return 1;
}