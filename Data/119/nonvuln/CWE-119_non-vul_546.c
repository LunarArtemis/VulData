int snd_usb_get_cur_mix_value(struct usb_mixer_elem_info *cval,
			     int channel, int index, int *value)
{
	int err;

	if (cval->cached & (1 << channel)) {
		*value = cval->cache_val[index];
		return 0;
	}
	err = get_cur_mix_raw(cval, channel, value);
	if (err < 0) {
		if (!cval->head.mixer->ignore_ctl_error)
			usb_audio_dbg(cval->head.mixer->chip,
				"cannot get current value for control %d ch %d: err = %d\n",
				      cval->control, channel, err);
		return err;
	}
	cval->cached |= 1 << channel;
	cval->cache_val[index] = *value;
	return 0;
}