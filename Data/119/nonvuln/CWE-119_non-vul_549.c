static void build_connector_control(struct usb_mixer_interface *mixer,
				    struct usb_audio_term *term, bool is_input)
{
	struct snd_kcontrol *kctl;
	struct usb_mixer_elem_info *cval;

	cval = kzalloc(sizeof(*cval), GFP_KERNEL);
	if (!cval)
		return;
	snd_usb_mixer_elem_init_std(&cval->head, mixer, term->id);
	/*
	 * UAC2: The first byte from reading the UAC2_TE_CONNECTOR control returns the
	 * number of channels connected.
	 *
	 * UAC3: The first byte specifies size of bitmap for the inserted controls. The
	 * following byte(s) specifies which connectors are inserted.
	 *
	 * This boolean ctl will simply report if any channels are connected
	 * or not.
	 */
	if (mixer->protocol == UAC_VERSION_2)
		cval->control = UAC2_TE_CONNECTOR;
	else /* UAC_VERSION_3 */
		cval->control = UAC3_TE_INSERTION;

	cval->val_type = USB_MIXER_BOOLEAN;
	cval->channels = 1; /* report true if any channel is connected */
	cval->min = 0;
	cval->max = 1;
	kctl = snd_ctl_new1(&usb_connector_ctl_ro, cval);
	if (!kctl) {
		usb_audio_err(mixer->chip, "cannot malloc kcontrol\n");
		kfree(cval);
		return;
	}
	get_connector_control_name(mixer, term, is_input, kctl->id.name,
				   sizeof(kctl->id.name));
	kctl->private_free = snd_usb_mixer_elem_free;
	snd_usb_mixer_add_control(&cval->head, kctl);
}