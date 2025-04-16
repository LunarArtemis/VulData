static int restore_mixer_value(struct usb_mixer_elem_list *list)
{
	struct usb_mixer_elem_info *cval = mixer_elem_list_to_info(list);
	int c, err, idx;

	if (cval->cmask) {
		idx = 0;
		for (c = 0; c < MAX_CHANNELS; c++) {
			if (!(cval->cmask & (1 << c)))
				continue;
			if (cval->cached & (1 << (c + 1))) {
				err = snd_usb_set_cur_mix_value(cval, c + 1, idx,
							cval->cache_val[idx]);
				if (err < 0)
					return err;
			}
			idx++;
		}
	} else {
		/* master */
		if (cval->cached) {
			err = snd_usb_set_cur_mix_value(cval, 0, 0, *cval->cache_val);
			if (err < 0)
				return err;
		}
	}

	return 0;
}