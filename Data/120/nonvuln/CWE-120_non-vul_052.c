LIBOPENMPT_MODPLUG_API void ModPlug_UnloadMixerCallback(ModPlugFile* file)
{
	if(!file) return;
	file->mixerproc = NULL;
	if(file->mixerbuf){
		free(file->mixerbuf);
		file->mixerbuf = NULL;
	}
}