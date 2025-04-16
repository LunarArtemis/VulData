static int mixer_ctl_master_bool_get(struct snd_kcontrol *kcontrol,
				     struct snd_ctl_elem_value *ucontrol)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	int val, err;

	err = snd_usb_get_cur_mix_value(cval, 0, 0, &val);
	if (err < 0)
		return filter_error(cval, err);
	val = (val != 0);
	ucontrol->value.integer.value[0] = val;
	return 0;
}