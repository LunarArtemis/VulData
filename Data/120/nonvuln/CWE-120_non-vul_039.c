LIBOPENMPT_MODPLUG_API void ModPlug_SeekOrder(ModPlugFile* file,int order)
{
	if(!file) return;
	openmpt_module_set_position_order_row(file->mod,order,0);
}