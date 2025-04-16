static int mixer_ctl_procunit_get(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	int err, val;

	err = get_cur_ctl_value(cval, cval->control << 8, &val);
	if (err < 0) {
		ucontrol->value.integer.value[0] = cval->min;
		return filter_error(cval, err);
	}
	val = get_relative_value(cval, val);
	ucontrol->value.integer.value[0] = val;
	return 0;
}