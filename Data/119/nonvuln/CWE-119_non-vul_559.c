static bool uac3_badd_func_has_valid_channels(struct usb_mixer_interface *mixer,
					      struct uac3_badd_profile *f,
					      int c_chmask, int p_chmask)
{
	/*
	 * If both playback/capture channels are dynamic, make sure
	 * at least one channel is present
	 */
	if (f->c_chmask < 0 && f->p_chmask < 0) {
		if (!c_chmask && !p_chmask) {
			usb_audio_warn(mixer->chip, "BAAD %s: no channels?",
				       f->name);
			return false;
		}
		return true;
	}

	if ((f->c_chmask < 0 && !c_chmask) ||
	    (f->c_chmask >= 0 && f->c_chmask != c_chmask)) {
		usb_audio_warn(mixer->chip, "BAAD %s c_chmask mismatch",
			       f->name);
		return false;
	}
	if ((f->p_chmask < 0 && !p_chmask) ||
	    (f->p_chmask >= 0 && f->p_chmask != p_chmask)) {
		usb_audio_warn(mixer->chip, "BAAD %s p_chmask mismatch",
			       f->name);
		return false;
	}
	return true;
}