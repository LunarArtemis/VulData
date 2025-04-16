LIBOPENMPT_MODPLUG_API void ModPlug_SetMasterVolume(ModPlugFile* file,unsigned int cvol)
{
	if(!file) return;
	openmpt_module_set_render_param(file->mod,OPENMPT_MODULE_RENDER_MASTERGAIN_MILLIBEL,(int32_t)(2000.0*log10(cvol/128.0)));
}