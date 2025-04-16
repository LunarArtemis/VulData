static void get_connector_control_name(struct usb_mixer_interface *mixer,
				       struct usb_audio_term *term,
				       bool is_input, char *name, int name_size)
{
	int name_len = get_term_name(mixer->chip, term, name, name_size, 0);

	if (name_len == 0)
		strlcpy(name, "Unknown", name_size);

	/*
	 *  sound/core/ctljack.c has a convention of naming jack controls
	 * by ending in " Jack".  Make it slightly more useful by
	 * indicating Input or Output after the terminal name.
	 */
	if (is_input)
		strlcat(name, " - Input Jack", name_size);
	else
		strlcat(name, " - Output Jack", name_size);
}