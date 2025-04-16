static int32_t modplugresamplingmode_to_filterlength(int mode)
{
	if(mode<0){
		return 1;
	}
	switch(mode){
	case MODPLUG_RESAMPLE_NEAREST: return 1; break;
	case MODPLUG_RESAMPLE_LINEAR: return 2; break;
	case MODPLUG_RESAMPLE_SPLINE: return 4; break;
	case MODPLUG_RESAMPLE_FIR: return 8; break;
	}
	return 8;
}