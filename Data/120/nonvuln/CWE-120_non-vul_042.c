int ModPlug_NumPatterns(ModPlugFile* file)
{
	if(!file) return 0;
	return openmpt_module_get_num_patterns(file->mod);
}