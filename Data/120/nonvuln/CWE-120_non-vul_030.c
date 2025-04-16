int ModPlug_NumChannels(ModPlugFile* file)
{
	if(!file) return 0;
	return openmpt_module_get_num_channels(file->mod);
}