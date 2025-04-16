void snd_usb_mixer_notify_id(struct usb_mixer_interface *mixer, int unitid)
{
	struct usb_mixer_elem_list *list;

	for_each_mixer_elem(list, mixer, unitid) {
		struct usb_mixer_elem_info *info =
			mixer_elem_list_to_info(list);
		/* invalidate cache, so the value is read from the device */
		info->cached = 0;
		snd_ctl_notify(mixer->chip->card, SNDRV_CTL_EVENT_MASK_VALUE,
			       &list->kctl->id);
	}
}