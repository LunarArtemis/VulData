static int parse_audio_extension_unit(struct mixer_build *state, int unitid,
				      void *raw_desc)
{
	/*
	 * Note that we parse extension units with processing unit descriptors.
	 * That's ok as the layout is the same.
	 */
	return build_audio_procunit(state, unitid, raw_desc, extunits, true);
}