int snd_usb_mixer_add_control(struct usb_mixer_elem_list *list,
			      struct snd_kcontrol *kctl)
{
	struct usb_mixer_interface *mixer = list->mixer;
	int err;

	while (snd_ctl_find_id(mixer->chip->card, &kctl->id))
		kctl->id.index++;
	err = snd_ctl_add(mixer->chip->card, kctl);
	if (err < 0) {
		usb_audio_dbg(mixer->chip, "cannot add control (err = %d)\n",
			      err);
		return err;
	}
	list->kctl = kctl;
	list->next_id_elem = mixer->id_elems[list->id];
	mixer->id_elems[list->id] = list;
	return 0;
}