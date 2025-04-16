static int parse_clock_source_unit(struct mixer_build *state, int unitid,
				   void *_ftr)
{
	struct uac_clock_source_descriptor *hdr = _ftr;
	struct usb_mixer_elem_info *cval;
	struct snd_kcontrol *kctl;
	char name[SNDRV_CTL_ELEM_ID_NAME_MAXLEN];
	int ret;

	if (state->mixer->protocol != UAC_VERSION_2)
		return -EINVAL;

	if (hdr->bLength != sizeof(*hdr)) {
		usb_audio_dbg(state->chip,
			      "Bogus clock source descriptor length of %d, ignoring.\n",
			      hdr->bLength);
		return 0;
	}

	/*
	 * The only property of this unit we are interested in is the
	 * clock source validity. If that isn't readable, just bail out.
	 */
	if (!uac_v2v3_control_is_readable(hdr->bmControls,
				      UAC2_CS_CONTROL_CLOCK_VALID))
		return 0;

	cval = kzalloc(sizeof(*cval), GFP_KERNEL);
	if (!cval)
		return -ENOMEM;

	snd_usb_mixer_elem_init_std(&cval->head, state->mixer, hdr->bClockID);

	cval->min = 0;
	cval->max = 1;
	cval->channels = 1;
	cval->val_type = USB_MIXER_BOOLEAN;
	cval->control = UAC2_CS_CONTROL_CLOCK_VALID;

	cval->master_readonly = 1;
	/* From UAC2 5.2.5.1.2 "Only the get request is supported." */
	kctl = snd_ctl_new1(&usb_bool_master_control_ctl_ro, cval);

	if (!kctl) {
		kfree(cval);
		return -ENOMEM;
	}

	kctl->private_free = snd_usb_mixer_elem_free;
	ret = snd_usb_copy_string_desc(state->chip, hdr->iClockSource,
				       name, sizeof(name));
	if (ret > 0)
		snprintf(kctl->id.name, sizeof(kctl->id.name),
			 "%s Validity", name);
	else
		snprintf(kctl->id.name, sizeof(kctl->id.name),
			 "Clock Source %d Validity", hdr->bClockID);

	return snd_usb_mixer_add_control(&cval->head, kctl);
}