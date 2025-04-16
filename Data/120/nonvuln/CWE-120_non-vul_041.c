LIBOPENMPT_MODPLUG_API int ModPlug_GetLength(ModPlugFile* file)
{
	if(!file) return 0;
	return (int)(openmpt_module_get_duration_seconds(file->mod)*1000.0);
}