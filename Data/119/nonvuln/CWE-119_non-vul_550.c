static void snd_usb_mixer_interrupt_v2(struct usb_mixer_interface *mixer,
				       int attribute, int value, int index)
{
	struct usb_mixer_elem_list *list;
	__u8 unitid = (index >> 8) & 0xff;
	__u8 control = (value >> 8) & 0xff;
	__u8 channel = value & 0xff;
	unsigned int count = 0;

	if (channel >= MAX_CHANNELS) {
		usb_audio_dbg(mixer->chip,
			"%s(): bogus channel number %d\n",
			__func__, channel);
		return;
	}

	for_each_mixer_elem(list, mixer, unitid)
		count++;

	if (count == 0)
		return;

	for_each_mixer_elem(list, mixer, unitid) {
		struct usb_mixer_elem_info *info;

		if (!list->kctl)
			continue;

		info = mixer_elem_list_to_info(list);
		if (count > 1 && info->control != control)
			continue;

		switch (attribute) {
		case UAC2_CS_CUR:
			/* invalidate cache, so the value is read from the device */
			if (channel)
				info->cached &= ~(1 << channel);
			else /* master channel */
				info->cached = 0;

			snd_ctl_notify(mixer->chip->card, SNDRV_CTL_EVENT_MASK_VALUE,
				       &info->head.kctl->id);
			break;

		case UAC2_CS_RANGE:
			/* TODO */
			break;

		case UAC2_CS_MEM:
			/* TODO */
			break;

		default:
			usb_audio_dbg(mixer->chip,
				"unknown attribute %d in interrupt\n",
				attribute);
			break;
		} /* switch */
	}
}