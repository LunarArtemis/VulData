static int parse_audio_input_terminal(struct mixer_build *state, int unitid,
				      void *raw_desc)
{
	struct usb_audio_term iterm;
	unsigned int control, bmctls, term_id;

	if (state->mixer->protocol == UAC_VERSION_2) {
		struct uac2_input_terminal_descriptor *d_v2 = raw_desc;
		if (d_v2->bLength < sizeof(*d_v2))
			return -EINVAL;
		control = UAC2_TE_CONNECTOR;
		term_id = d_v2->bTerminalID;
		bmctls = le16_to_cpu(d_v2->bmControls);
	} else if (state->mixer->protocol == UAC_VERSION_3) {
		struct uac3_input_terminal_descriptor *d_v3 = raw_desc;
		if (d_v3->bLength < sizeof(*d_v3))
			return -EINVAL;
		control = UAC3_TE_INSERTION;
		term_id = d_v3->bTerminalID;
		bmctls = le32_to_cpu(d_v3->bmControls);
	} else {
		return 0; /* UAC1. No Insertion control */
	}

	check_input_term(state, term_id, &iterm);

	/* Check for jack detection. */
	if (uac_v2v3_control_is_readable(bmctls, control))
		build_connector_control(state->mixer, &iterm, true);

	return 0;
}