LIBOPENMPT_MODPLUG_API void ModPlug_InitMixerCallback(ModPlugFile* file,ModPlugMixerProc proc)
{
	if(!file) return;
	if(!file->mixerbuf){
		file->mixerbuf = malloc(BUFFER_COUNT*sizeof(signed int)*4);
	}
	file->mixerproc = proc;
}