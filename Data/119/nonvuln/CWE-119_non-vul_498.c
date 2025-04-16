static int mixer_ctl_feature_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	int c, cnt, val, err;

	ucontrol->value.integer.value[0] = cval->min;
	if (cval->cmask) {
		cnt = 0;
		for (c = 0; c < MAX_CHANNELS; c++) {
			if (!(cval->cmask & (1 << c)))
				continue;
			err = snd_usb_get_cur_mix_value(cval, c + 1, cnt, &val);
			if (err < 0)
				return filter_error(cval, err);
			val = get_relative_value(cval, val);
			ucontrol->value.integer.value[cnt] = val;
			cnt++;
		}
		return 0;
	} else {
		/* master channel */
		err = snd_usb_get_cur_mix_value(cval, 0, 0, &val);
		if (err < 0)
			return filter_error(cval, err);
		val = get_relative_value(cval, val);
		ucontrol->value.integer.value[0] = val;
	}
	return 0;
}