static int mixer_ctl_feature_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	int c, cnt, val, oval, err;
	int changed = 0;

	if (cval->cmask) {
		cnt = 0;
		for (c = 0; c < MAX_CHANNELS; c++) {
			if (!(cval->cmask & (1 << c)))
				continue;
			err = snd_usb_get_cur_mix_value(cval, c + 1, cnt, &oval);
			if (err < 0)
				return filter_error(cval, err);
			val = ucontrol->value.integer.value[cnt];
			val = get_abs_value(cval, val);
			if (oval != val) {
				snd_usb_set_cur_mix_value(cval, c + 1, cnt, val);
				changed = 1;
			}
			cnt++;
		}
	} else {
		/* master channel */
		err = snd_usb_get_cur_mix_value(cval, 0, 0, &oval);
		if (err < 0)
			return filter_error(cval, err);
		val = ucontrol->value.integer.value[0];
		val = get_abs_value(cval, val);
		if (val != oval) {
			snd_usb_set_cur_mix_value(cval, 0, 0, val);
			changed = 1;
		}
	}
	return changed;
}