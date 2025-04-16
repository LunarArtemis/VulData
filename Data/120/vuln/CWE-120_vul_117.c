static Bool gf_eac3_parser_internal(GF_BitStream *bs, GF_AC3Config *hdr, Bool full_parse)
{
	u32 fscod, bsid, acmod, freq, framesize, syncword, substreamid, lfon, numblkscod, strmtyp, frmsiz, bsmod;
	u64 pos, hdr_pos;
	u16 chanmap;
	Bool main_indep_found = GF_FALSE;
	s32 cur_main_id = -1;
	u32 nb_blocks_main;
	u32 cur_main_ac3 = 0;
	u16 main_substreams; //bit-mask of independent channels found so far
	static u32 numblks[4] = {1, 2, 3, 6};

	if (!hdr || !AC3_FindSyncCodeBS(bs))
		return GF_FALSE;

retry_frame:
	pos = gf_bs_get_position(bs);
	framesize = 0;
	numblkscod = 0;
	bsmod = 0;
	nb_blocks_main = 0;
	main_substreams = 0;
	memset(hdr, 0, sizeof(GF_AC3Config));

next_block:
	hdr_pos = gf_bs_get_position(bs);

	bsid = gf_bs_peek_bits(bs, 5, 5);
	//"If an AC-3 bit stream is present in the Enhanced AC-3 bit stream, then the AC-3 bit stream shall be treated
	//as an independent substream assigned substream ID 0."
	if (bsid<=8) {
		GF_AC3Header ac3h;
		//we are done
		if (main_indep_found) {
			eac3_update_channels(hdr);
			gf_bs_seek(bs, pos);
			return GF_TRUE;
		}
		if (!gf_ac3_parser_bs(bs, &ac3h, GF_TRUE)) {
			gf_bs_seek(bs, pos);
			return GF_FALSE;
		}
		hdr->streams[0] = ac3h.streams[0];
		hdr->nb_streams = 1;
		hdr->sample_rate = ac3h.sample_rate;
		main_substreams |= 1;
		hdr->framesize = ac3h.framesize;
		nb_blocks_main = 6;
		hdr->brcode = gf_ac3_get_bitrate(ac3h.brcode)/1000;

		gf_bs_skip_bytes(bs, ac3h.framesize);
		if (!AC3_FindSyncCodeBS(bs)) {
			gf_bs_seek(bs, pos);
			return GF_FALSE;
		}
		main_indep_found = GF_TRUE;
		cur_main_id = 0;
		cur_main_ac3 = 1;
		goto next_block;
	}
	//corrupted frame, trash
	if ((bsid<10) || (bsid>16)) {
		gf_bs_skip_bytes(bs, 1);//we are still at the startcode
		if (!AC3_FindSyncCodeBS(bs)) {
			gf_bs_seek(bs, pos);
			return GF_FALSE;
		}
		goto next_block;
	}
	syncword = gf_bs_read_u16(bs);
	if (syncword != 0x0B77) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[E-AC3] Wrong sync word detected (0x%X - expecting 0x0B77).\n", syncword));
		return GF_FALSE;
	}

	hdr->is_ec3 = 1;
	strmtyp = gf_bs_read_int_log(bs, 2, "strmtyp");
	substreamid = gf_bs_read_int_log(bs, 3, "substreamid");

	//independent stream
	if (strmtyp!=0x1) {
		cur_main_ac3 = 0;
		//all blocks gathered and we have seen this substreamid, done with whole frame
		if ( (nb_blocks_main==6) && ( (main_substreams >> substreamid) & 0x1)) {
			eac3_update_channels(hdr);
			gf_bs_seek(bs, pos);
			return GF_TRUE;
		}
		//main independent: "All Enhanced AC-3 bit streams shall contain an independent substream assigned substream ID 0.
		// The independent substream assigned substream ID 0 shall be the first substream present in the bit stream."
		if (!substreamid)
			main_indep_found = 1;
		if (cur_main_id != substreamid)
			nb_blocks_main=0;
		cur_main_id = substreamid;
	}

	//trash everything until we find first indep stream
	if (!main_indep_found) {
		gf_bs_align(bs);
		if (!AC3_FindSyncCodeBS(bs)) {
			gf_bs_seek(bs, pos);
			return GF_FALSE;
		}
		goto retry_frame;
	}
	//quick hack (not sure if the spec forbids this): some AC3+EAC3 streams use substreamid=0 for the eac3
	//which breaks nb_dep_sub / chan_loc signaling
	//we increase by one in this case
	if (cur_main_ac3 && !substreamid) cur_main_ac3=2;
	if (cur_main_ac3==2) substreamid++;

	frmsiz = gf_bs_read_int_log(bs, 11, "frmsiz");
	framesize += 2 * (1 + frmsiz);
	fscod = gf_bs_read_int_log(bs, 2, "fscod");
	if (fscod == 0x3) {
		fscod = gf_bs_read_int_log(bs, 2, "fscod2");
		numblkscod = 3;
	} else {
		numblkscod = gf_bs_read_int_log(bs, 2, "numblkscod");
	}

	//remember our independent substreams
	if (strmtyp!=0x1) {
		main_substreams |= (1 << substreamid);
	}

	switch (fscod) {
	case 0:
		freq = 48000;
		break;
	case 1:
		freq = 44100;
		break;
	case 2:
		freq = 32000;
		break;
	default:
		//do not sync
		gf_bs_align(bs);
		return GF_FALSE;
	}

	acmod = gf_bs_read_int_log(bs, 3, "acmod");
	lfon = gf_bs_read_int_log(bs, 1, "lfon");
	bsid = gf_bs_read_int_log(bs, 5, "bsid");

	gf_bs_read_int_log(bs, 5, "dialnorm");
	if (gf_bs_read_int_log(bs, 1, "compre")) {
		gf_bs_read_int_log(bs, 8, "compr");
	}
	if (acmod==0) {
		gf_bs_read_int_log(bs, 5, "dialnorm2");
		if (gf_bs_read_int_log(bs, 1, "compr2e")) {
			gf_bs_read_int_log(bs, 8, "compr2");
		}
	}
	chanmap = 0;
	if (strmtyp==0x1) {
		if (gf_bs_read_int_log(bs, 1, "chanmape")) {
			chanmap = gf_bs_read_int_log(bs, 16, "chanmap");
		}
	}

	hdr->sample_rate = freq;
	hdr->framesize += framesize;
	if (strmtyp != 1) {
		assert(cur_main_id == substreamid);
		hdr->streams[substreamid].lfon = lfon;
		hdr->streams[substreamid].bsid = bsid;
		hdr->streams[substreamid].bsmod = bsmod;
		hdr->streams[substreamid].acmod = acmod;
		hdr->streams[substreamid].fscod = fscod;
		hdr->brcode = 0;
		hdr->nb_streams++;
	}
	//dependent stream, record max substream ID of dep and store chan map
	else {
		hdr->streams[cur_main_id].nb_dep_sub = substreamid;
		hdr->streams[cur_main_id].chan_loc |= chanmap;
	}

	//not clear if this is only for the independent streams - spec says "The value is the sum of the data rates of all the substreams"
	hdr->brcode += ((frmsiz+1) * freq) / (numblks[numblkscod]*16) / 1000;

	//start of header only, we are done - chan info might be wrong
	if (!full_parse) {
		eac3_update_channels(hdr);
		gf_bs_seek(bs, pos);
		return GF_TRUE;
	}

	//mix metadata
	if (gf_bs_read_int(bs, 1)) {
		if (acmod > 0x2) gf_bs_read_int(bs, 2);
		if ((acmod & 0x1) && (acmod > 0x2)) gf_bs_read_int(bs, 6);
		if (acmod & 0x4) gf_bs_read_int(bs, 6);
		if (lfon) {
			if (gf_bs_read_int(bs, 1))
				gf_bs_read_int(bs, 5);
		}
		if (strmtyp == 0) {
			//pgmscle
			if (gf_bs_read_int(bs, 1))
				gf_bs_read_int(bs, 6);
			if (acmod==0) {
				//pgmscl2e
				if (gf_bs_read_int(bs, 1))
					gf_bs_read_int(bs, 6);
			}
			//extpgmscle
			if (gf_bs_read_int(bs, 1))
				gf_bs_read_int(bs, 6);
			u8 mixdef = gf_bs_read_int(bs, 2);
			if (mixdef == 0x1) {
				gf_bs_read_int(bs, 5);
			} else if (mixdef == 0x2) {
				gf_bs_read_int(bs, 12);
			} else if (mixdef == 0x3) {
				u32 mixdeflen = gf_bs_read_int(bs, 5);
				mixdeflen = 8 * (mixdeflen + 2);
				while (mixdeflen) {
					gf_bs_read_int(bs, 1);
					mixdeflen--;
				}
			}
			if (acmod < 0x2) {
				//paninfoe
				if (gf_bs_read_int(bs, 1))
					gf_bs_read_int(bs, 14);
				if (acmod == 0) {
					//paninfo2e
					if (gf_bs_read_int(bs, 1))
						gf_bs_read_int(bs, 14);
				}

			}
			//frmmixcfginfoe
			if (gf_bs_read_int(bs, 1)) {
				if (numblkscod == 0x0) {
					gf_bs_read_int(bs, 5);
				} else {
					u32 i, nb_blocks = numblks[numblkscod];
					for (i=0; i<nb_blocks; i++) {
						if (gf_bs_read_int(bs, 1))
							gf_bs_read_int(bs, 5);
					}
				}
			}
		}
	}
	//info metadata
	if (gf_bs_read_int(bs, 1)) {
		gf_bs_read_int(bs, 5);
		if (acmod == 0x2) gf_bs_read_int(bs, 4);
		if (acmod >= 0x6) gf_bs_read_int(bs, 2);
		//audprodie
		if (gf_bs_read_int(bs, 1)) gf_bs_read_int(bs, 8);
		if (acmod == 0x0) {
			//audprodi2e
			if (gf_bs_read_int(bs, 1)) gf_bs_read_int(bs, 8);
		}
		if (fscod < 0x3)  gf_bs_read_int(bs, 1);
	}
	if ((strmtyp == 0) && (numblkscod != 0x3)) gf_bs_read_int(bs, 1);
	if (strmtyp == 0x2) {
		u32 blkid=0;
		if (numblkscod == 0x3) blkid=1;
		else blkid = gf_bs_read_int(bs, 1);
		if (blkid) gf_bs_read_int(bs, 6);
	}
	u8 addbsie = gf_bs_read_int(bs, 1);
	if (addbsie) {
		u32 addbsil = gf_bs_read_int(bs, 6) + 1;
		//we only use the first 2 bytes - cf 8.3 of ETSI 103 420 V1.2.1
		if (addbsil>=2) {
			gf_bs_read_int(bs, 7);
			if (gf_bs_read_int(bs, 1)) {
				hdr->atmos_ec3_ext = 1;
				hdr->complexity_index_type = gf_bs_read_int(bs, 8);
			}
		}
	}

	//remember numbers of block for main
	if (strmtyp!=0x1) {
		nb_blocks_main += numblks[numblkscod];
	}

	if (gf_bs_seek(bs, hdr_pos + framesize) != GF_OK) {
		gf_bs_seek(bs, pos);
		return GF_FALSE;
	}

	if (!AC3_FindSyncCodeBS(bs)) {
		gf_bs_seek(bs, pos);
		return GF_FALSE;
	}
	//we go to next block even if we have 6 of main (to check deps)
	goto next_block;
}