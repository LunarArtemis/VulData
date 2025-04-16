LIBOPENMPT_MODPLUG_API void ModPlug_SetSettings(const ModPlug_Settings* settings)
{
	if(!settings) return;
	memcpy(&globalsettings,settings,sizeof(ModPlug_Settings));
}