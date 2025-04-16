static int keep_iface_ctl_get(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_interface *mixer = snd_kcontrol_chip(kcontrol);

	ucontrol->value.integer.value[0] = mixer->chip->keep_iface;
	return 0;
}