LIBOPENMPT_MODPLUG_API ModPlugFile* ModPlug_Load(const void* data, int size)
{
	ModPlugFile* file = malloc(sizeof(ModPlugFile));
	const char* name = NULL;
	const char* message = NULL;
	if(!file) return NULL;
	memset(file,0,sizeof(ModPlugFile));
	memcpy(&file->settings,&globalsettings,sizeof(ModPlug_Settings));
	file->mod = openmpt_module_create_from_memory2(data,size,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	if(!file->mod){
		free(file);
		return NULL;
	}
	file->buf = malloc(BUFFER_COUNT*sizeof(signed short)*4);
	if(!file->buf){
		openmpt_module_destroy(file->mod);
		free(file);
		return NULL;
	}
	openmpt_module_set_repeat_count(file->mod,file->settings.mLoopCount);
	name = openmpt_module_get_metadata(file->mod,"title");
	if(name){
		file->name = malloc(strlen(name)+1);
		if(file->name){
			strcpy(file->name,name);
		}
		openmpt_free_string(name);
		name = NULL;
	}else{
		file->name = malloc(strlen("")+1);
		if(file->name){
			strcpy(file->name,"");
		}
	}
	message = openmpt_module_get_metadata(file->mod,"message");
	if(message){
		file->message = malloc(strlen(message)+1);
		if(file->message){
			strcpy(file->message,message);
		}
		openmpt_free_string(message);
		message = NULL;
	}else{
		file->message = malloc(strlen("")+1);
		if(file->message){
			strcpy(file->message,"");
		}
	}
	openmpt_module_set_render_param(file->mod,OPENMPT_MODULE_RENDER_STEREOSEPARATION_PERCENT,file->settings.mStereoSeparation*100/128);
	openmpt_module_set_render_param(file->mod,OPENMPT_MODULE_RENDER_INTERPOLATIONFILTER_LENGTH,modplugresamplingmode_to_filterlength(file->settings.mResamplingMode));
	return file;
}