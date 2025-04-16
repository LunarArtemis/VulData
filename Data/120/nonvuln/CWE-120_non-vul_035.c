LIBOPENMPT_MODPLUG_API int ModPlug_GetCurrentTempo(ModPlugFile* file)
{
	if(!file) return 0;
	return openmpt_module_get_current_tempo(file->mod);
}