static void build_feature_ctl(struct mixer_build *state, void *raw_desc,
			      unsigned int ctl_mask, int control,
			      struct usb_audio_term *iterm, int unitid,
			      int readonly_mask)
{
	struct uac_feature_unit_descriptor *desc = raw_desc;
	int nameid = uac_feature_unit_iFeature(desc);

	__build_feature_ctl(state->mixer, state->map, ctl_mask, control,
			iterm, &state->oterm, unitid, nameid, readonly_mask);
}