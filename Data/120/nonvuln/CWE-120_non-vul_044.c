LIBOPENMPT_MODPLUG_API int ModPlug_GetModuleType(ModPlugFile* file)
{
	const char* type;
	int retval;
	if(!file) return 0;
	type = openmpt_module_get_metadata(file->mod,"type");
	retval = MOD_TYPE_NONE;
	if(!type){
		return retval;
	}
	if(!strcmp(type,"mod")){
		retval = MOD_TYPE_MOD;
	}else if(!strcmp(type,"s3m")){
		retval = MOD_TYPE_S3M;
	}else if(!strcmp(type,"xm")){
		retval = MOD_TYPE_XM;
	}else if(!strcmp(type,"med")){
		retval = MOD_TYPE_MED;
	}else if(!strcmp(type,"mtm")){
		retval = MOD_TYPE_MTM;
	}else if(!strcmp(type,"it")){
		retval = MOD_TYPE_IT;
	}else if(!strcmp(type,"669")){
		retval = MOD_TYPE_669;
	}else if(!strcmp(type,"ult")){
		retval = MOD_TYPE_ULT;
	}else if(!strcmp(type,"stm")){
		retval = MOD_TYPE_STM;
	}else if(!strcmp(type,"far")){
		retval = MOD_TYPE_FAR;
	}else if(!strcmp(type,"s3m")){
		retval = MOD_TYPE_WAV;
	}else if(!strcmp(type,"amf")){
		retval = MOD_TYPE_AMF;
	}else if(!strcmp(type,"ams")){
		retval = MOD_TYPE_AMS;
	}else if(!strcmp(type,"dsm")){
		retval = MOD_TYPE_DSM;
	}else if(!strcmp(type,"mdl")){
		retval = MOD_TYPE_MDL;
	}else if(!strcmp(type,"okt")){
		retval = MOD_TYPE_OKT;
	}else if(!strcmp(type,"mid")){
		retval = MOD_TYPE_MID;
	}else if(!strcmp(type,"dmf")){
		retval = MOD_TYPE_DMF;
	}else if(!strcmp(type,"ptm")){
		retval = MOD_TYPE_PTM;
	}else if(!strcmp(type,"dbm")){
		retval = MOD_TYPE_DBM;
	}else if(!strcmp(type,"mt2")){
		retval = MOD_TYPE_MT2;
	}else if(!strcmp(type,"amf0")){
		retval = MOD_TYPE_AMF0;
	}else if(!strcmp(type,"psm")){
		retval = MOD_TYPE_PSM;
	}else if(!strcmp(type,"j2b")){
		retval = MOD_TYPE_J2B;
	}else if(!strcmp(type,"abc")){
		retval = MOD_TYPE_ABC;
	}else if(!strcmp(type,"pat")){
		retval = MOD_TYPE_PAT;
	}else if(!strcmp(type,"umx")){
		retval = MOD_TYPE_UMX;
	}else{
		retval = MOD_TYPE_IT; /* fallback, most complex type */
	}
	openmpt_free_string(type);
	return retval;
}