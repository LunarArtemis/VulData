int ModPlug_GetMasterVolume(ModPlugFile* file)
{
	int32_t val;
	if(!file) return 0;
	val = 0;
	if(!openmpt_module_get_render_param(file->mod,OPENMPT_MODULE_RENDER_MASTERGAIN_MILLIBEL,&val)) return 128;
	return (unsigned int)(128.0*pow(10.0,val*0.0005));
}