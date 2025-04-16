LIBOPENMPT_MODPLUG_API void ModPlug_Seek(ModPlugFile* file, int millisecond)
{
	if(!file) return;
	openmpt_module_set_position_seconds(file->mod,(double)millisecond*0.001);
}