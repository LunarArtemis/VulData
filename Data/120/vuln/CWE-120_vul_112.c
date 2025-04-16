static s32 naludmx_parse_nal_avc(GF_NALUDmxCtx *ctx, char *data, u32 size, u32 nal_type, Bool *skip_nal, Bool *is_slice, Bool *is_islice)
{
	s32 ps_idx = 0;
	s32 res = 0;

	if (!size) return -1;
	gf_bs_reassign_buffer(ctx->bs_r, data, size);
	*skip_nal = GF_FALSE;
	res = gf_avc_parse_nalu(ctx->bs_r, ctx->avc_state);
	if (res < 0) {
		if (res == -1) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Warning: Error parsing NAL unit\n", ctx->log_name));
		}
		*skip_nal = GF_TRUE;
	}
	ctx->nb_nalus++;

	switch (nal_type) {
	case GF_AVC_NALU_SVC_SUBSEQ_PARAM:
	case GF_AVC_NALU_SEQ_PARAM:
		ps_idx = ctx->avc_state->last_ps_idx;
		if (ps_idx<0) {
			if (ctx->avc_state->sps[0].profile_idc) {
				GF_LOG(ctx->avc_state->sps[0].profile_idc ? GF_LOG_WARNING : GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing Sequence Param Set\n", ctx->log_name));
			}
		} else {
			naludmx_queue_param_set(ctx, data, size, GF_AVC_NALU_SEQ_PARAM, ps_idx, 0, 0);
		}
		*skip_nal = GF_TRUE;
		return 0;

	case GF_AVC_NALU_PIC_PARAM:
		ps_idx = ctx->avc_state->last_ps_idx;
		if (ps_idx<0) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing Picture Param Set\n", ctx->log_name));
		} else {
			naludmx_queue_param_set(ctx, data, size, GF_AVC_NALU_PIC_PARAM, ps_idx, 0, 0);
		}
		*skip_nal = GF_TRUE;
		return 0;

	case GF_AVC_NALU_SEQ_PARAM_EXT:
		ps_idx = ctx->avc_state->last_ps_idx;
		if (ps_idx<0) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing Sequence Param Set Extension\n", ctx->log_name));
		} else {
			naludmx_queue_param_set(ctx, data, size, GF_AVC_NALU_SEQ_PARAM_EXT, ps_idx, 0, 0);
		}
		*skip_nal = GF_TRUE;
		return 0;

	case GF_AVC_NALU_SEI:
		if (ctx->avc_state->sps_active_idx != -1) {
			naludmx_push_prefix(ctx, data, size, GF_TRUE);

			*skip_nal = GF_TRUE;

			if (ctx->nosei) {
				ctx->sei_buffer_size = 0;
			} else {
				ctx->nb_sei++;
			}
		}
		return 0;

	case GF_AVC_NALU_ACCESS_UNIT:
		ctx->nb_aud++;
		if (!ctx->audelim) {
			*skip_nal = GF_TRUE;
		} else if (!ctx->opid) {
			ctx->has_initial_aud = GF_TRUE;
			memcpy(ctx->init_aud, data, 2);
		}
		return 1;
	/*remove*/
	case GF_AVC_NALU_FILLER_DATA:
	case GF_AVC_NALU_END_OF_SEQ:
	case GF_AVC_NALU_END_OF_STREAM:
		*skip_nal = GF_TRUE;
		return 0;

	//update stats
	case GF_AVC_NALU_NON_IDR_SLICE:
	case GF_AVC_NALU_DP_A_SLICE:
	case GF_AVC_NALU_DP_B_SLICE:
	case GF_AVC_NALU_DP_C_SLICE:
	case GF_AVC_NALU_IDR_SLICE:
		*is_slice = GF_TRUE;
		switch (ctx->avc_state->s_info.slice_type) {
		case GF_AVC_TYPE_P:
		case GF_AVC_TYPE2_P:
			ctx->nb_p++;
			break;
		case GF_AVC_TYPE_I:
		case GF_AVC_TYPE2_I:
			ctx->nb_i++;
			*is_islice = GF_TRUE;
			break;
		case GF_AVC_TYPE_B:
		case GF_AVC_TYPE2_B:
			ctx->nb_b++;
			break;
		case GF_AVC_TYPE_SP:
		case GF_AVC_TYPE2_SP:
			ctx->nb_sp++;
			break;
		case GF_AVC_TYPE_SI:
		case GF_AVC_TYPE2_SI:
			ctx->nb_si++;
			break;
		}
		break;

	case GF_AVC_NALU_SVC_SLICE:
		if (!ctx->explicit) {
			u32 i;
			for (i = 0; i < gf_list_count(ctx->pps); i ++) {
				GF_NALUFFParam *slc = (GF_NALUFFParam*)gf_list_get(ctx->pps, i);
				if (ctx->avc_state->s_info.pps && ctx->avc_state->s_info.pps->id == slc->id) {
					/* This PPS is used by an SVC NAL unit, it should be moved to the SVC Config Record) */
					gf_list_rem(ctx->pps, i);
					i--;
					if (!ctx->pps_svc) ctx->pps_svc = gf_list_new(ctx->pps_svc);
					gf_list_add(ctx->pps_svc, slc);
					ctx->ps_modified = GF_TRUE;
				}
			}
		}
		*is_slice = GF_TRUE;
		//we disable temporal scalability when parsing mvc - never used and many encoders screw up POC in enhancemen
		if (ctx->is_mvc && (res>=0)) {
			res=0;
			ctx->avc_state->s_info.poc = ctx->last_poc;
		}
        if (ctx->avc_state->s_info.sps) {
            switch (ctx->avc_state->s_info.slice_type) {
            case GF_AVC_TYPE_P:
            case GF_AVC_TYPE2_P:
                ctx->avc_state->s_info.sps->nb_ep++;
                break;
            case GF_AVC_TYPE_I:
            case GF_AVC_TYPE2_I:
                ctx->avc_state->s_info.sps->nb_ei++;
                break;
            case GF_AVC_TYPE_B:
            case GF_AVC_TYPE2_B:
                ctx->avc_state->s_info.sps->nb_eb++;
                break;
            }
        }
        break;
	case GF_AVC_NALU_SLICE_AUX:
		*is_slice = GF_TRUE;
		break;

	case GF_AVC_NALU_DV_RPU:
		if (ctx->dv_mode==DVMODE_CLEAN) {
			*skip_nal = GF_TRUE;
		} else {
			ctx->nb_dv_rpu ++;
			if (ctx->nb_dv_rpu==1)
				naludmx_set_dolby_vision(ctx);
		}
		break;
	case GF_AVC_NALU_DV_EL:
		if ((ctx->dv_mode==DVMODE_CLEAN) || (ctx->dv_mode==DVMODE_SINGLE)) {
			*skip_nal = GF_TRUE;
		} else {
			ctx->nb_dv_el ++;
			if (ctx->nb_dv_el==1)
				naludmx_set_dolby_vision(ctx);
		}
		break;
	}
	return res;
}