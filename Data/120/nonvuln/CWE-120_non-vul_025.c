LIBOPENMPT_MODPLUG_API ModPlugNote* ModPlug_GetPattern(ModPlugFile* file, int pattern, unsigned int* numrows)
{
	int c;
	int r;
	int numr;
	int numc;
	ModPlugNote note;
	if(!file) return NULL;
	if(numrows){
		*numrows = openmpt_module_get_pattern_num_rows(file->mod,pattern);
	}
	if(pattern<0||pattern>=openmpt_module_get_num_patterns(file->mod)){
		return NULL;
	}
	if(!file->patterns){
		file->patterns = malloc(sizeof(ModPlugNote*)*openmpt_module_get_pattern_num_rows(file->mod,pattern));
		if(!file->patterns) return NULL;
		memset(file->patterns,0,sizeof(ModPlugNote*)*openmpt_module_get_pattern_num_rows(file->mod,pattern));
	}
	if(!file->patterns[pattern]){
		file->patterns[pattern] = malloc(sizeof(ModPlugNote)*openmpt_module_get_pattern_num_rows(file->mod,pattern)*openmpt_module_get_num_channels(file->mod));
		if(!file->patterns[pattern]) return NULL;
		memset(file->patterns[pattern],0,sizeof(ModPlugNote)*openmpt_module_get_pattern_num_rows(file->mod,pattern)*openmpt_module_get_num_channels(file->mod));
	}
	numr = openmpt_module_get_pattern_num_rows(file->mod,pattern);
	numc = openmpt_module_get_num_channels(file->mod);
	for(r=0;r<numr;r++){
		for(c=0;c<numc;c++){
			memset(&note,0,sizeof(ModPlugNote));
			note.Note = openmpt_module_get_pattern_row_channel_command(file->mod,pattern,r,c,OPENMPT_MODULE_COMMAND_NOTE);
			note.Instrument = openmpt_module_get_pattern_row_channel_command(file->mod,pattern,r,c,OPENMPT_MODULE_COMMAND_INSTRUMENT);
			note.VolumeEffect = openmpt_module_get_pattern_row_channel_command(file->mod,pattern,r,c,OPENMPT_MODULE_COMMAND_VOLUMEEFFECT);
			note.Effect = openmpt_module_get_pattern_row_channel_command(file->mod,pattern,r,c,OPENMPT_MODULE_COMMAND_EFFECT);
			note.Volume = openmpt_module_get_pattern_row_channel_command(file->mod,pattern,r,c,OPENMPT_MODULE_COMMAND_VOLUME);
			note.Parameter = openmpt_module_get_pattern_row_channel_command(file->mod,pattern,r,c,OPENMPT_MODULE_COMMAND_PARAMETER);
			memcpy(&file->patterns[pattern][r*numc+c],&note,sizeof(ModPlugNote));
		}
	}
	return file->patterns[pattern];
}