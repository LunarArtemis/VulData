LIBOPENMPT_MODPLUG_API int ModPlug_Read(ModPlugFile* file, void* buffer, int size)
{
	int framesize;
	int framecount;
	int frames;
	int rendered;
	int frame;
	int channel;
	int totalrendered;
	signed short* in;
	signed int* mixbuf;
	unsigned char* buf8;
	signed short* buf16;
	signed int* buf32;
	if(!file) return 0;
	framesize = file->settings.mBits/8*file->settings.mChannels;
	framecount = size/framesize;
	buf8 = buffer;
	buf16 = buffer;
	buf32 = buffer;
	totalrendered = 0;
	while(framecount>0){
		frames = framecount;
		if(frames>BUFFER_COUNT){
			frames = BUFFER_COUNT;
		}
		if(file->settings.mChannels==1){
			rendered = (int)openmpt_module_read_mono(file->mod,file->settings.mFrequency,frames,&file->buf[frames*0]);
		}else if(file->settings.mChannels==2){
			rendered = (int)openmpt_module_read_stereo(file->mod,file->settings.mFrequency,frames,&file->buf[frames*0],&file->buf[frames*1]);
		}else if(file->settings.mChannels==4){
			rendered = (int)openmpt_module_read_quad(file->mod,file->settings.mFrequency,frames,&file->buf[frames*0],&file->buf[frames*1],&file->buf[frames*2],&file->buf[frames*3]);
		}else{
			return 0;
		}
		in = file->buf;
		if(file->mixerproc&&file->mixerbuf){
			mixbuf=file->mixerbuf;
			for(frame=0;frame<frames;frame++){
				for(channel=0;channel<file->settings.mChannels;channel++){
					*mixbuf = in[frames*channel+frame]<<(32-16-1-MIXING_ATTENUATION);
					mixbuf++;
				}
			}
			file->mixerproc(file->mixerbuf,file->settings.mChannels*frames,file->settings.mChannels);
			mixbuf=file->mixerbuf;
			for(frame=0;frame<frames;frame++){
				for(channel=0;channel<file->settings.mChannels;channel++){
					in[frames*channel+frame] = *mixbuf>>(32-16-1-MIXING_ATTENUATION);
					mixbuf++;
				}
			}
		}
		if(file->settings.mBits==8){
			for(frame=0;frame<frames;frame++){
				for(channel=0;channel<file->settings.mChannels;channel++){
					*buf8 = in[frames*channel+frame]/256+0x80;
					buf8++;
				}
			}
		}else if(file->settings.mBits==16){
			for(frame=0;frame<frames;frame++){
				for(channel=0;channel<file->settings.mChannels;channel++){
					*buf16 = in[frames*channel+frame];
					buf16++;
				}
			}
		}else if(file->settings.mBits==32){
			for(frame=0;frame<frames;frame++){
				for(channel=0;channel<file->settings.mChannels;channel++){
					*buf32 = in[frames*channel+frame] << (32-16-1-MIXING_ATTENUATION);
					buf32++;
				}
			}
		}else{
			return 0;
		}
		totalrendered += rendered;
		framecount -= frames;
		if(!rendered) break;
	}
	memset(((char*)buffer)+totalrendered*framesize,0,size-totalrendered*framesize);
	return totalrendered*framesize;
}