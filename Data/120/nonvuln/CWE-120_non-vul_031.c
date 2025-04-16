LIBOPENMPT_MODPLUG_API int ModPlug_GetPlayingChannels(ModPlugFile* file)
{
	if(!file) return 0;
	return openmpt_module_get_current_playing_channels(file->mod);
}