static void build_mixer_unit_ctl(struct mixer_build *state,
				 struct uac_mixer_unit_descriptor *desc,
				 int in_pin, int in_ch, int num_outs,
				 int unitid, struct usb_audio_term *iterm)
{
	struct usb_mixer_elem_info *cval;
	unsigned int i, len;
	struct snd_kcontrol *kctl;
	const struct usbmix_name_map *map;

	map = find_map(state->map, unitid, 0);
	if (check_ignored_ctl(map))
		return;

	cval = kzalloc(sizeof(*cval), GFP_KERNEL);
	if (!cval)
		return;

	snd_usb_mixer_elem_init_std(&cval->head, state->mixer, unitid);
	cval->control = in_ch + 1; /* based on 1 */
	cval->val_type = USB_MIXER_S16;
	for (i = 0; i < num_outs; i++) {
		__u8 *c = uac_mixer_unit_bmControls(desc, state->mixer->protocol);

		if (check_matrix_bitmap(c, in_ch, i, num_outs)) {
			cval->cmask |= (1 << i);
			cval->channels++;
		}
	}

	/* get min/max values */
	get_min_max(cval, 0);

	kctl = snd_ctl_new1(&usb_feature_unit_ctl, cval);
	if (!kctl) {
		usb_audio_err(state->chip, "cannot malloc kcontrol\n");
		kfree(cval);
		return;
	}
	kctl->private_free = snd_usb_mixer_elem_free;

	len = check_mapped_name(map, kctl->id.name, sizeof(kctl->id.name));
	if (!len)
		len = get_term_name(state->chip, iterm, kctl->id.name,
				    sizeof(kctl->id.name), 0);
	if (!len)
		len = sprintf(kctl->id.name, "Mixer Source %d", in_ch + 1);
	append_ctl_name(kctl, " Volume");

	usb_audio_dbg(state->chip, "[%d] MU [%s] ch = %d, val = %d/%d\n",
		    cval->head.id, kctl->id.name, cval->channels, cval->min, cval->max);
	snd_usb_mixer_add_control(&cval->head, kctl);
}