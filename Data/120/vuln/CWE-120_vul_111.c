static s32 naludmx_parse_nal_hevc(GF_NALUDmxCtx *ctx, char *data, u32 size, Bool *skip_nal, Bool *is_slice, Bool *is_islice)
{
#ifdef GPAC_DISABLE_HEVC
	return -1;
#else
	s32 ps_idx = 0;
	s32 res;
	u8 nal_unit_type, temporal_id, layer_id;
	*skip_nal = GF_FALSE;

	if (size<2) return -1;

	gf_bs_reassign_buffer(ctx->bs_r, data, size);
	res = gf_hevc_parse_nalu_bs(ctx->bs_r, ctx->hevc_state, &nal_unit_type, &temporal_id, &layer_id);
	ctx->nb_nalus++;

	if (res < 0) {
		if (res == -1) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Warning: Error parsing NAL unit\n", ctx->log_name));
		}
		*skip_nal = GF_TRUE;
	}

	if (layer_id && ctx->nosvc) {
		*skip_nal = GF_TRUE;
		return 0;
	}

	switch (nal_unit_type) {
	case GF_HEVC_NALU_VID_PARAM:
		if (ctx->novpsext) {
			//this may modify nal_size, but we don't use it for bitstream reading
			ps_idx = gf_hevc_read_vps_ex(data, &size, ctx->hevc_state, GF_TRUE);
		} else {
			ps_idx = ctx->hevc_state->last_parsed_vps_id;
		}
		if (ps_idx<0) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing Video Param Set\n", ctx->log_name));
		} else {
			naludmx_queue_param_set(ctx, data, size, GF_HEVC_NALU_VID_PARAM, ps_idx, temporal_id, layer_id);
		}
		*skip_nal = GF_TRUE;
		break;
	case GF_HEVC_NALU_SEQ_PARAM:
		ps_idx = ctx->hevc_state->last_parsed_sps_id;
		if (ps_idx<0) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing Sequence Param Set\n", ctx->log_name));
		} else {
			naludmx_queue_param_set(ctx, data, size, GF_HEVC_NALU_SEQ_PARAM, ps_idx, temporal_id, layer_id);
		}
		*skip_nal = GF_TRUE;
		break;
	case GF_HEVC_NALU_PIC_PARAM:
		ps_idx = ctx->hevc_state->last_parsed_pps_id;
		if (ps_idx<0) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing Picture Param Set\n", ctx->log_name));
		} else {
			naludmx_queue_param_set(ctx, data, size, GF_HEVC_NALU_PIC_PARAM, ps_idx, temporal_id, layer_id);
		}
		*skip_nal = GF_TRUE;
		break;
	case GF_HEVC_NALU_SEI_PREFIX:
		gf_hevc_parse_sei(data, size, ctx->hevc_state);
		if (!ctx->nosei) {
			ctx->nb_sei++;
			naludmx_push_prefix(ctx, data, size, GF_FALSE);
		} else {
			ctx->nb_nalus--;
		}
		*skip_nal = GF_TRUE;
		break;
	case GF_HEVC_NALU_SEI_SUFFIX:
		if (! ctx->is_playing) return 0;
		if (ctx->nosei) {
			*skip_nal = GF_TRUE;
			ctx->nb_nalus--;
		} else {
			ctx->nb_sei++;
		}
		break;

	/*slice_segment_layer_rbsp*/
	case GF_HEVC_NALU_SLICE_STSA_N:
	case GF_HEVC_NALU_SLICE_STSA_R:
	case GF_HEVC_NALU_SLICE_RADL_R:
	case GF_HEVC_NALU_SLICE_RASL_R:
	case GF_HEVC_NALU_SLICE_RADL_N:
	case GF_HEVC_NALU_SLICE_RASL_N:
	case GF_HEVC_NALU_SLICE_TRAIL_N:
	case GF_HEVC_NALU_SLICE_TRAIL_R:
	case GF_HEVC_NALU_SLICE_TSA_N:
	case GF_HEVC_NALU_SLICE_TSA_R:
	case GF_HEVC_NALU_SLICE_BLA_W_LP:
	case GF_HEVC_NALU_SLICE_BLA_W_DLP:
	case GF_HEVC_NALU_SLICE_BLA_N_LP:
	case GF_HEVC_NALU_SLICE_IDR_W_DLP:
	case GF_HEVC_NALU_SLICE_IDR_N_LP:
	case GF_HEVC_NALU_SLICE_CRA:
		if (! ctx->is_playing) return 0;
		*is_slice = GF_TRUE;
		ctx->last_layer_id = layer_id;
		ctx->last_temporal_id = temporal_id;
		if (! *skip_nal) {
			switch (ctx->hevc_state->s_info.slice_type) {
			case GF_HEVC_SLICE_TYPE_P:
				if (layer_id) ctx->nb_e_p++;
				else ctx->nb_p++;
				break;
			case GF_HEVC_SLICE_TYPE_I:
				if (layer_id) ctx->nb_e_i++;
				else ctx->nb_i++;
				*is_islice = GF_TRUE;
				break;
			case GF_HEVC_SLICE_TYPE_B:
				if (layer_id) ctx->nb_e_b++;
				else ctx->nb_b++;
				break;
			}
		}
		break;

	case GF_HEVC_NALU_ACCESS_UNIT:
		ctx->nb_aud++;
		if (!ctx->audelim) {
			*skip_nal = GF_TRUE;
		} else if (!ctx->opid) {
			ctx->has_initial_aud = GF_TRUE;
			memcpy(ctx->init_aud, data, 3);
		}
		break;
	/*remove*/
	case GF_HEVC_NALU_FILLER_DATA:
	case GF_HEVC_NALU_END_OF_SEQ:
	case GF_HEVC_NALU_END_OF_STREAM:
		*skip_nal = GF_TRUE;
		break;

	//parsing is partial, see https://github.com/DolbyLaboratories/dlb_mp4base/blob/70a2e1d4d99a8439b7b8087bf50dd503eeea2291/src/esparser/parser_hevc.c#L1233
	case GF_HEVC_NALU_DV_RPU:
		if (ctx->dv_mode==DVMODE_CLEAN) {
			*skip_nal = GF_TRUE;
		} else {
			ctx->nb_dv_rpu ++;
			if (ctx->nb_dv_rpu==1)
				naludmx_set_dolby_vision(ctx);
		}
		break;
	case GF_HEVC_NALU_DV_EL:
		if ((ctx->dv_mode==DVMODE_CLEAN) || (ctx->dv_mode==DVMODE_SINGLE)) {
			*skip_nal = GF_TRUE;
		} else {
			ctx->nb_dv_el ++;
			if (ctx->nb_dv_el==1)
				naludmx_set_dolby_vision(ctx);
		}
		break;

	default:
		if (! ctx->is_playing) return 0;
		GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[%s] NAL Unit type %d not handled - adding\n", ctx->log_name, nal_unit_type));
		break;
	}
	if (*skip_nal) return res;

	ctx->linf[layer_id].layer_id_plus_one = layer_id + 1;
	if (! ctx->linf[layer_id].max_temporal_id ) ctx->linf[layer_id].max_temporal_id = temporal_id;
	else if (ctx->linf[layer_id].max_temporal_id < temporal_id) ctx->linf[layer_id].max_temporal_id = temporal_id;

	if (! ctx->linf[layer_id].min_temporal_id ) ctx->linf[layer_id].min_temporal_id = temporal_id;
	else if (ctx->linf[layer_id].min_temporal_id > temporal_id) ctx->linf[layer_id].min_temporal_id = temporal_id;

	if (ctx->max_temporal_id[layer_id] < temporal_id)
		ctx->max_temporal_id[layer_id] = temporal_id;
	if (ctx->min_layer_id > layer_id) ctx->min_layer_id = layer_id;
	return res;
#endif // GPAC_DISABLE_HEVC
}