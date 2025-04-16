LIBOPENMPT_MODPLUG_API char ModPlug_ExportIT(ModPlugFile* file, const char* filepath)
{
	(void)file;
	/* not implemented */
	fprintf(stderr,"libopenmpt-modplug: error: ModPlug_ExportIT(%s) not implemented.\n",filepath);
	return 0;
}