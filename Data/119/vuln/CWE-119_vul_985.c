int t2p_process_jpeg_strip(
	unsigned char* strip, 
	tsize_t* striplength, 
	unsigned char* buffer, 
	tsize_t* bufferoffset, 
	tstrip_t no, 
	uint32 height){

	tsize_t i=0;

	while (i < *striplength) {
		tsize_t datalen;
		uint16 ri;
		uint16 v_samp;
		uint16 h_samp;
		int j;
		int ncomp;

		/* marker header: one or more FFs */
		if (strip[i] != 0xff)
			return(0);
		i++;
		while (i < *striplength && strip[i] == 0xff)
			i++;
		if (i >= *striplength)
			return(0);
		/* SOI is the only pre-SOS marker without a length word */
		if (strip[i] == 0xd8)
			datalen = 0;
		else {
			if ((*striplength - i) <= 2)
				return(0);
			datalen = (strip[i+1] << 8) | strip[i+2];
			if (datalen < 2 || datalen >= (*striplength - i))
				return(0);
		}
		switch( strip[i] ){
			case 0xd8:	/* SOI - start of image */
				_TIFFmemcpy(&(buffer[*bufferoffset]), &(strip[i-1]), 2);
				*bufferoffset+=2;
				break;
			case 0xc0:	/* SOF0 */
			case 0xc1:	/* SOF1 */
			case 0xc3:	/* SOF3 */
			case 0xc9:	/* SOF9 */
			case 0xca:	/* SOF10 */
				if(no==0){
					_TIFFmemcpy(&(buffer[*bufferoffset]), &(strip[i-1]), datalen+2);
					ncomp = buffer[*bufferoffset+9];
					if (ncomp < 1 || ncomp > 4)
						return(0);
					v_samp=1;
					h_samp=1;
					for(j=0;j<ncomp;j++){
						uint16 samp = buffer[*bufferoffset+11+(3*j)];
						if( (samp>>4) > h_samp) 
							h_samp = (samp>>4);
						if( (samp & 0x0f) > v_samp) 
							v_samp = (samp & 0x0f);
					}
					v_samp*=8;
					h_samp*=8;
					ri=((( ((uint16)(buffer[*bufferoffset+5])<<8) | 
					(uint16)(buffer[*bufferoffset+6]) )+v_samp-1)/ 
					v_samp);
					ri*=((( ((uint16)(buffer[*bufferoffset+7])<<8) | 
					(uint16)(buffer[*bufferoffset+8]) )+h_samp-1)/ 
					h_samp);
					buffer[*bufferoffset+5]=
                                          (unsigned char) ((height>>8) & 0xff);
					buffer[*bufferoffset+6]=
                                            (unsigned char) (height & 0xff);
					*bufferoffset+=datalen+2;
					/* insert a DRI marker */
					buffer[(*bufferoffset)++]=0xff;
					buffer[(*bufferoffset)++]=0xdd;
					buffer[(*bufferoffset)++]=0x00;
					buffer[(*bufferoffset)++]=0x04;
					buffer[(*bufferoffset)++]=(ri >> 8) & 0xff;
					buffer[(*bufferoffset)++]= ri & 0xff;
				}
				break;
			case 0xc4: /* DHT */
			case 0xdb: /* DQT */
				_TIFFmemcpy(&(buffer[*bufferoffset]), &(strip[i-1]), datalen+2);
				*bufferoffset+=datalen+2;
				break;
			case 0xda: /* SOS */
				if(no==0){
					_TIFFmemcpy(&(buffer[*bufferoffset]), &(strip[i-1]), datalen+2);
					*bufferoffset+=datalen+2;
				} else {
					buffer[(*bufferoffset)++]=0xff;
					buffer[(*bufferoffset)++]=
                                            (unsigned char)(0xd0 | ((no-1)%8));
				}
				i += datalen + 1;
				/* copy remainder of strip */
				_TIFFmemcpy(&(buffer[*bufferoffset]), &(strip[i]), *striplength - i);
				*bufferoffset+= *striplength - i;
				return(1);
			default:
				/* ignore any other marker */
				break;
		}
		i += datalen + 1;
	}

	/* failed to find SOS marker */
	return(0);
}