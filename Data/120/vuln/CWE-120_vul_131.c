GF_Err h263dmx_process(GF_Filter *filter)
{
	GF_H263DmxCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck, *dst_pck;
	u64 byte_offset;
	char *data;
	u8 *start;
	Bool first_frame_found = GF_FALSE;
	u32 pck_size;
	s32 remain;

	//always reparse duration
	if (!ctx->duration.num)
		h263dmx_check_dur(filter, ctx);

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!pck) {
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			if (ctx->opid)
				gf_filter_pid_set_eos(ctx->opid);
			if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
			ctx->src_pck = NULL;
			return GF_EOS;
		}
		return GF_OK;
	}

	data = (char *) gf_filter_pck_get_data(pck, &pck_size);
	byte_offset = gf_filter_pck_get_byte_offset(pck);

	start = data;
	remain = pck_size;


	if (ctx->bytes_in_header) {
#if 0
		if (ctx->bytes_in_header + remain < 7) {
			memcpy(ctx->header + ctx->bytes_in_header, start, remain);
			ctx->bytes_in_header += remain;
			gf_filter_pid_drop_packet(ctx->ipid);
			return GF_OK;
		}
		alread_sync = 7 - ctx->bytes_in_header;
		memcpy(ctx->header + ctx->bytes_in_header, start, alread_sync);
		start += alread_sync;
		remain -= alread_sync;
		ctx->bytes_in_header = 0;
		alread_sync = GF_TRUE;
#endif

	}
	//input pid is muxed - we flushed pending data , update cts unless recomputing timing
	else if (ctx->timescale) {
		if (!ctx->notime) {
			u64 cts = gf_filter_pck_get_cts(pck);
			if (cts != GF_FILTER_NO_TS)
				ctx->cts = cts;
		}
		if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
		ctx->src_pck = pck;
		gf_filter_pck_ref_props(&ctx->src_pck);
	}

	if (ctx->resume_from) {
		if (gf_filter_pid_would_block(ctx->opid))
			return GF_OK;
		start += ctx->resume_from;
		remain -= ctx->resume_from;
		ctx->resume_from = 0;
	}

	while (remain) {
		u32 size=0;
		Bool full_frame;
		u8 *pck_data;
		s32 current, next;
		u32 fmt, w, h;

		//not enough bytes
		if (remain<5) {
			memcpy(ctx->hdr_store, start, remain);
			ctx->bytes_in_header = remain;
			break;
		}

		if (ctx->bytes_in_header) {
			if (first_frame_found) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[H263Dmx] corrupted frame!\n"));
			}

			memcpy(ctx->hdr_store + ctx->bytes_in_header, start, 8 - ctx->bytes_in_header);
			current = h263dmx_next_start_code(ctx->hdr_store, 8);

			//no start code in stored buffer
			if (current<0) {
				dst_pck = gf_filter_pck_new_alloc(ctx->opid, ctx->bytes_in_header, &pck_data);
				if (!dst_pck) return GF_OUT_OF_MEM;

				if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, dst_pck);

				memcpy(pck_data, ctx->hdr_store, ctx->bytes_in_header);
				gf_filter_pck_set_framing(dst_pck, GF_FALSE, GF_FALSE);
				gf_filter_pck_set_cts(dst_pck, ctx->cts);
				gf_filter_pck_set_duration(dst_pck, ctx->fps.den);
				if (ctx->in_seek) gf_filter_pck_set_seek_flag(dst_pck, GF_TRUE);

				if (byte_offset != GF_FILTER_NO_BO) {
					gf_filter_pck_set_byte_offset(dst_pck, byte_offset - ctx->bytes_in_header);
				}
				gf_filter_pck_send(dst_pck);
				ctx->bytes_in_header = 0;

				current = h263dmx_next_start_code(start, remain);
			}
		} else {
			//locate next start code
			current = h263dmx_next_start_code(start, remain);
		}


		if (current<0) {
			//not enough bytes to process start code !!
			break;
		}

		if (current>0) {
			if (!ctx->opid) {
				if (ctx->bytes_in_header) {
					ctx->bytes_in_header -= current;
				} else {
					start += current;
					remain -= current;
				}
				GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[H263Dmx] garbage before first frame!\n"));
				continue;
			}
			if (first_frame_found) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[H263Dmx] corrupted frame!\n"));
			}
			//flush remaining
			dst_pck = gf_filter_pck_new_alloc(ctx->opid, current, &pck_data);
			if (!dst_pck) return GF_OUT_OF_MEM;

			if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, dst_pck);

			if (ctx->bytes_in_header) {
				if (byte_offset != GF_FILTER_NO_BO) {
					gf_filter_pck_set_byte_offset(dst_pck, byte_offset - ctx->bytes_in_header);
				}
				ctx->bytes_in_header -= current;
				memcpy(pck_data, ctx->hdr_store, current);
			} else {
				if (byte_offset != GF_FILTER_NO_BO) {
					gf_filter_pck_set_byte_offset(dst_pck, byte_offset);
				}
				memcpy(pck_data, start, current);
				start += current;
				remain -= current;
			}
			gf_filter_pck_set_framing(dst_pck, GF_FALSE, GF_TRUE);
			gf_filter_pck_set_cts(dst_pck, ctx->cts);
			gf_filter_pck_set_duration(dst_pck, ctx->fps.den);
			if (ctx->in_seek) gf_filter_pck_set_seek_flag(dst_pck, GF_TRUE);
			gf_filter_pck_send(dst_pck);

			h263dmx_update_cts(ctx);
		}

		if (ctx->bytes_in_header) {
			gf_bs_reassign_buffer(ctx->bs, ctx->hdr_store+current, 8-current);
		} else if (!ctx->bs) {
			ctx->bs = gf_bs_new(start, remain, GF_BITSTREAM_READ);
		} else {
			gf_bs_reassign_buffer(ctx->bs, start, remain);
		}
		/*parse header*/
		gf_bs_read_int(ctx->bs, 22);
		gf_bs_read_int(ctx->bs, 8);
		/*spare+0+split_screen_indicator+document_camera_indicator+freeze_picture_release*/
		gf_bs_read_int(ctx->bs, 5);

		fmt = gf_bs_read_int(ctx->bs, 3);
		h263_get_pic_size(ctx->bs, fmt, &w, &h);

		h263dmx_check_pid(filter, ctx, w, h);

		if (!ctx->is_playing) {
			ctx->resume_from = (u32) ( (char *)start -  (char *)data );
			return GF_OK;
		}

		if (ctx->in_seek) {
			u64 nb_frames_at_seek = (u64) (ctx->start_range * ctx->fps.num);
			if (ctx->cts + ctx->fps.den >= nb_frames_at_seek) {
				//u32 samples_to_discard = (ctx->cts + ctx->dts_inc) - nb_samples_at_seek;
				ctx->in_seek = GF_FALSE;
			}
		}

		//good to go
		next = h263dmx_next_start_code(start+1, remain-1);

		if (next>0) {
			size = next+1 + ctx->bytes_in_header;
			full_frame = GF_TRUE;
		} else {
			u8 b3 = start[remain-3];
			u8 b2 = start[remain-2];
			u8 b1 = start[remain-1];
			//we may have a startcode here !
			if (!b1 || !b2 || !b3) {
				memcpy(ctx->hdr_store, start+remain-3, 3);
				remain -= 3;
				ctx->bytes_in_header = 3;
			}
			size = remain;
			full_frame = GF_FALSE;
		}

		dst_pck = gf_filter_pck_new_alloc(ctx->opid, size, &pck_data);
		if (!dst_pck) return GF_OUT_OF_MEM;

		if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, dst_pck);
		if (ctx->bytes_in_header && current) {
			memcpy(pck_data, ctx->hdr_store+current, ctx->bytes_in_header);
			size -= ctx->bytes_in_header;
			ctx->bytes_in_header = 0;
			if (byte_offset != GF_FILTER_NO_BO) {
				gf_filter_pck_set_byte_offset(dst_pck, byte_offset + ctx->bytes_in_header);
			}
			memcpy(pck_data, start, size);
		} else {
			memcpy(pck_data, start, size);
			if (byte_offset != GF_FILTER_NO_BO) {
				gf_filter_pck_set_byte_offset(dst_pck, byte_offset + start - (u8 *) data);
			}
		}

		gf_filter_pck_set_framing(dst_pck, GF_TRUE, full_frame);
		gf_filter_pck_set_cts(dst_pck, ctx->cts);
		gf_filter_pck_set_sap(dst_pck, (start[4]&0x02) ? GF_FILTER_SAP_NONE : GF_FILTER_SAP_1);
		gf_filter_pck_set_duration(dst_pck, ctx->fps.den);
		if (ctx->in_seek) gf_filter_pck_set_seek_flag(dst_pck, GF_TRUE);
		gf_filter_pck_send(dst_pck);

		first_frame_found = GF_TRUE;
		start += size;
		remain -= size;
		if (!full_frame) break;
		h263dmx_update_cts(ctx);


		//don't demux too much of input, abort when we would block. This avoid dispatching
		//a huge number of frames in a single call
		if (gf_filter_pid_would_block(ctx->opid)) {
			ctx->resume_from = (u32) ( (char *)start -  (char *)data);
			return GF_OK;
		}
	}
	gf_filter_pid_drop_packet(ctx->ipid);

	return GF_OK;
}