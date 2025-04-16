LIBOPENMPT_MODPLUG_API char ModPlug_ExportMOD(ModPlugFile* file, const char* filepath)
{
	(void)file;
	/* not implemented */
	fprintf(stderr,"libopenmpt-modplug: error: ModPlug_ExportMOD(%s) not implemented.\n",filepath);
	return 0;
}