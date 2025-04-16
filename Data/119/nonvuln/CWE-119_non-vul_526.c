static void snd_usb_mixer_proc_read(struct snd_info_entry *entry,
				    struct snd_info_buffer *buffer)
{
	struct snd_usb_audio *chip = entry->private_data;
	struct usb_mixer_interface *mixer;
	struct usb_mixer_elem_list *list;
	int unitid;

	list_for_each_entry(mixer, &chip->mixer_list, list) {
		snd_iprintf(buffer,
			"USB Mixer: usb_id=0x%08x, ctrlif=%i, ctlerr=%i\n",
				chip->usb_id, snd_usb_ctrl_intf(chip),
				mixer->ignore_ctl_error);
		snd_iprintf(buffer, "Card: %s\n", chip->card->longname);
		for (unitid = 0; unitid < MAX_ID_ELEMS; unitid++) {
			for_each_mixer_elem(list, mixer, unitid) {
				snd_iprintf(buffer, "  Unit: %i\n", list->id);
				if (list->kctl)
					snd_iprintf(buffer,
						    "    Control: name=\"%s\", index=%i\n",
						    list->kctl->id.name,
						    list->kctl->id.index);
				if (list->dump)
					list->dump(buffer, list);
			}
		}
	}
}