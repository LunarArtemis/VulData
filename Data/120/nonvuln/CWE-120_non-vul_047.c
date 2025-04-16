LIBOPENMPT_MODPLUG_API const char* ModPlug_GetName(ModPlugFile* file)
{
	if(!file) return NULL;
	return file->name;
}