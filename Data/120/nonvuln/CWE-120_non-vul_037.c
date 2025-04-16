LIBOPENMPT_MODPLUG_API char* ModPlug_GetMessage(ModPlugFile* file)
{
	if(!file) return NULL;
	return file->message;
}