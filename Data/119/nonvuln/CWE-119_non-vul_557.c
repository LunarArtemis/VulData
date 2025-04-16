static int mixer_ctl_selector_info(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_info *uinfo)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	const char **itemlist = (const char **)kcontrol->private_value;

	if (snd_BUG_ON(!itemlist))
		return -EINVAL;
	return snd_ctl_enum_info(uinfo, 1, cval->max, itemlist);
}