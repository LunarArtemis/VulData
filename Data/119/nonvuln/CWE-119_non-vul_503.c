int snd_usb_set_cur_mix_value(struct usb_mixer_elem_info *cval, int channel,
			     int index, int value)
{
	int err;
	unsigned int read_only = (channel == 0) ?
		cval->master_readonly :
		cval->ch_readonly & (1 << (channel - 1));

	if (read_only) {
		usb_audio_dbg(cval->head.mixer->chip,
			      "%s(): channel %d of control %d is read_only\n",
			    __func__, channel, cval->control);
		return 0;
	}

	err = snd_usb_mixer_set_ctl_value(cval,
					  UAC_SET_CUR, (cval->control << 8) | channel,
					  value);
	if (err < 0)
		return err;
	cval->cached |= 1 << channel;
	cval->cache_val[index] = value;
	return 0;
}