int snd_usb_mixer_resume(struct usb_mixer_interface *mixer, bool reset_resume)
{
	struct usb_mixer_elem_list *list;
	int id, err;

	if (reset_resume) {
		/* restore cached mixer values */
		for (id = 0; id < MAX_ID_ELEMS; id++) {
			for_each_mixer_elem(list, mixer, id) {
				if (list->resume) {
					err = list->resume(list);
					if (err < 0)
						return err;
				}
			}
		}
	}

	snd_usb_mixer_resume_quirk(mixer);

	return snd_usb_mixer_activate(mixer);
}