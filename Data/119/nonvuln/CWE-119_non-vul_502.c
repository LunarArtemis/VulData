static int get_min_max_with_quirks(struct usb_mixer_elem_info *cval,
				   int default_min, struct snd_kcontrol *kctl)
{
	/* for failsafe */
	cval->min = default_min;
	cval->max = cval->min + 1;
	cval->res = 1;
	cval->dBmin = cval->dBmax = 0;

	if (cval->val_type == USB_MIXER_BOOLEAN ||
	    cval->val_type == USB_MIXER_INV_BOOLEAN) {
		cval->initialized = 1;
	} else {
		int minchn = 0;
		if (cval->cmask) {
			int i;
			for (i = 0; i < MAX_CHANNELS; i++)
				if (cval->cmask & (1 << i)) {
					minchn = i + 1;
					break;
				}
		}
		if (get_ctl_value(cval, UAC_GET_MAX, (cval->control << 8) | minchn, &cval->max) < 0 ||
		    get_ctl_value(cval, UAC_GET_MIN, (cval->control << 8) | minchn, &cval->min) < 0) {
			usb_audio_err(cval->head.mixer->chip,
				      "%d:%d: cannot get min/max values for control %d (id %d)\n",
				   cval->head.id, snd_usb_ctrl_intf(cval->head.mixer->chip),
							       cval->control, cval->head.id);
			return -EINVAL;
		}
		if (get_ctl_value(cval, UAC_GET_RES,
				  (cval->control << 8) | minchn,
				  &cval->res) < 0) {
			cval->res = 1;
		} else {
			int last_valid_res = cval->res;

			while (cval->res > 1) {
				if (snd_usb_mixer_set_ctl_value(cval, UAC_SET_RES,
								(cval->control << 8) | minchn,
								cval->res / 2) < 0)
					break;
				cval->res /= 2;
			}
			if (get_ctl_value(cval, UAC_GET_RES,
					  (cval->control << 8) | minchn, &cval->res) < 0)
				cval->res = last_valid_res;
		}
		if (cval->res == 0)
			cval->res = 1;

		/* Additional checks for the proper resolution
		 *
		 * Some devices report smaller resolutions than actually
		 * reacting.  They don't return errors but simply clip
		 * to the lower aligned value.
		 */
		if (cval->min + cval->res < cval->max) {
			int last_valid_res = cval->res;
			int saved, test, check;
			get_cur_mix_raw(cval, minchn, &saved);
			for (;;) {
				test = saved;
				if (test < cval->max)
					test += cval->res;
				else
					test -= cval->res;
				if (test < cval->min || test > cval->max ||
				    snd_usb_set_cur_mix_value(cval, minchn, 0, test) ||
				    get_cur_mix_raw(cval, minchn, &check)) {
					cval->res = last_valid_res;
					break;
				}
				if (test == check)
					break;
				cval->res *= 2;
			}
			snd_usb_set_cur_mix_value(cval, minchn, 0, saved);
		}

		cval->initialized = 1;
	}

	if (kctl)
		volume_control_quirks(cval, kctl);

	/* USB descriptions contain the dB scale in 1/256 dB unit
	 * while ALSA TLV contains in 1/100 dB unit
	 */
	cval->dBmin = (convert_signed_value(cval, cval->min) * 100) / 256;
	cval->dBmax = (convert_signed_value(cval, cval->max) * 100) / 256;
	if (cval->dBmin > cval->dBmax) {
		/* something is wrong; assume it's either from/to 0dB */
		if (cval->dBmin < 0)
			cval->dBmax = 0;
		else if (cval->dBmin > 0)
			cval->dBmin = 0;
		if (cval->dBmin > cval->dBmax) {
			/* totally crap, return an error */
			return -EINVAL;
		}
	}

	return 0;
}