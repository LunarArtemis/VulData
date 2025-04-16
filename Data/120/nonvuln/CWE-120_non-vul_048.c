LIBOPENMPT_MODPLUG_API void ModPlug_GetSettings(ModPlug_Settings* settings)
{
	if(!settings) return;
	memcpy(settings,&globalsettings,sizeof(ModPlug_Settings));
}