static int parse_audio_processing_unit(struct mixer_build *state, int unitid,
				       void *raw_desc)
{
	switch (state->mixer->protocol) {
	case UAC_VERSION_1:
	case UAC_VERSION_2:
	default:
		return build_audio_procunit(state, unitid, raw_desc,
					    procunits, false);
	case UAC_VERSION_3:
		return build_audio_procunit(state, unitid, raw_desc,
					    uac3_procunits, false);
	}
}