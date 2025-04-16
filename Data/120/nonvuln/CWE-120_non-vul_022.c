int ModPlug_NumInstruments(ModPlugFile* file)
{
	if(!file) return 0;
	return openmpt_module_get_num_instruments(file->mod);
}