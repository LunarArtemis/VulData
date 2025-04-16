LIBOPENMPT_MODPLUG_API char ModPlug_ExportXM(ModPlugFile* file, const char* filepath)
{
	(void)file;
	/* not implemented */
	fprintf(stderr,"libopenmpt-modplug: error: ModPlug_ExportXM(%s) not implemented.\n",filepath);
	return 0;
}