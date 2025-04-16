LIBOPENMPT_MODPLUG_API void ModPlug_Unload(ModPlugFile* file)
{
	int p;
	if(!file) return;
	if(file->patterns){
		for(p=0;p<openmpt_module_get_num_patterns(file->mod);p++){
			if(file->patterns[p]){
				free(file->patterns[p]);
				file->patterns[p] = NULL;
			}
		}
		free(file->patterns);
		file->patterns = NULL;
	}
	if(file->mixerbuf){
		free(file->mixerbuf);
		file->mixerbuf = NULL;
	}
	openmpt_module_destroy(file->mod);
	file->mod = NULL;
	free(file->name);
	file->name = NULL;
	free(file->message);
	file->message = NULL;
	free(file->buf);
	file->buf = NULL;
	free(file);
}