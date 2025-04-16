static int mixer_ctl_selector_get(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	int val, err;

	err = get_cur_ctl_value(cval, cval->control << 8, &val);
	if (err < 0) {
		ucontrol->value.enumerated.item[0] = 0;
		return filter_error(cval, err);
	}
	val = get_relative_value(cval, val);
	ucontrol->value.enumerated.item[0] = val;
	return 0;
}