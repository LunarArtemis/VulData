static void naludmx_create_hevc_decoder_config(GF_NALUDmxCtx *ctx, u8 **dsi, u32 *dsi_size, u8 **dsi_enh, u32 *dsi_enh_size, u32 *max_width, u32 *max_height, u32 *max_enh_width, u32 *max_enh_height, GF_Fraction *sar, Bool *has_hevc_base)
{
#ifndef GPAC_DISABLE_HEVC
	u32 i, count;
	u8 layer_id;
	Bool first = GF_TRUE;
	Bool first_lhvc = GF_TRUE;
	GF_HEVCConfig *cfg;
	GF_HEVCConfig *hvcc;
	GF_HEVCConfig *lvcc;
	u32 max_w, max_h, max_ew, max_eh;

	*has_hevc_base = GF_FALSE;


	max_w = max_h = 0;
	max_ew = max_eh = 0;
	sar->num = sar->den = 0;

	hvcc = gf_odf_hevc_cfg_new();
	lvcc = gf_odf_hevc_cfg_new();
	hvcc->nal_unit_size = ctx->nal_length;
	lvcc->nal_unit_size = ctx->nal_length;
	lvcc->is_lhvc = GF_TRUE;
	//check we have one pps or sps in base layer
	count = gf_list_count(ctx->sps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->sps, i);
		layer_id = ((sl->data[0] & 0x1) << 5) | (sl->data[1] >> 3);
		if (!layer_id) {
			*has_hevc_base = GF_TRUE;
			break;
		}
	}
	count = gf_list_count(ctx->pps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->pps, i);
		layer_id = ((sl->data[0] & 0x1) << 5) | (sl->data[1] >> 3);
		if (!layer_id) {
			*has_hevc_base = GF_TRUE;
			break;
		}
	}
	//assign vps first so that they are serialized first
	count = gf_list_count(ctx->vps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->vps, i);
		HEVC_VPS *vps = &ctx->hevc_state->vps[sl->id];

		if (!i) {
			hvcc->avgFrameRate = lvcc->avgFrameRate = vps->rates[0].avg_pic_rate;
			hvcc->constantFrameRate = lvcc->constantFrameRate = vps->rates[0].constant_pic_rate_idc;
			hvcc->numTemporalLayers = lvcc->numTemporalLayers = vps->max_sub_layers;
			hvcc->temporalIdNested = lvcc->temporalIdNested = vps->temporal_id_nesting;
		}
		//TODO set scalability mask
		if (!ctx->analyze)
			naludmx_add_param_nalu((ctx->explicit || ! (*has_hevc_base) ) ? lvcc->param_array : hvcc->param_array, sl, GF_HEVC_NALU_VID_PARAM);
	}

	count = gf_list_count(ctx->sps);
	for (i=0; i<count; i++) {
		Bool is_lhvc = GF_FALSE;
		GF_NALUFFParam *sl = gf_list_get(ctx->sps, i);
		HEVC_SPS *sps = &ctx->hevc_state->sps[sl->id];
		layer_id = ((sl->data[0] & 0x1) << 5) | (sl->data[1] >> 3);
		if (!layer_id) *has_hevc_base = GF_TRUE;

		if (ctx->explicit || layer_id) {
			cfg = lvcc;
			is_lhvc = GF_TRUE;
		} else {
			cfg = hvcc;
		}

		if (first || (is_lhvc && first_lhvc) ) {
			cfg->configurationVersion = 1;
			cfg->profile_space = sps->ptl.profile_space;
			cfg->tier_flag = sps->ptl.tier_flag;
			cfg->profile_idc = sps->ptl.profile_idc;
			cfg->general_profile_compatibility_flags = sps->ptl.profile_compatibility_flag;
			cfg->progressive_source_flag = sps->ptl.general_progressive_source_flag;
			cfg->interlaced_source_flag = sps->ptl.general_interlaced_source_flag;
			cfg->non_packed_constraint_flag = sps->ptl.general_non_packed_constraint_flag;
			cfg->frame_only_constraint_flag = sps->ptl.general_frame_only_constraint_flag;
			cfg->constraint_indicator_flags = sps->ptl.general_reserved_44bits;
			cfg->level_idc = sps->ptl.level_idc;
			cfg->chromaFormat = sps->chroma_format_idc;
			cfg->luma_bit_depth = sps->bit_depth_luma;
			cfg->chroma_bit_depth = sps->bit_depth_chroma;
			ctx->interlaced = cfg->interlaced_source_flag ? GF_TRUE : GF_FALSE;

			if (sps->aspect_ratio_info_present_flag && sps->sar_width && sps->sar_height) {
				sar->num = sps->sar_width;
				sar->den = sps->sar_height;
			}

			/*disable frame rate scan, most bitstreams have wrong values there*/
			if (ctx->notime && first && (!ctx->fps.num || !ctx->fps.den) && sps->has_timing_info
				/*if detected FPS is greater than 1000, assume wrong timing info*/
				&& (sps->time_scale <= 1000*sps->num_units_in_tick)
			) {
				ctx->cur_fps.num = sps->time_scale;
				ctx->cur_fps.den = sps->num_units_in_tick;

				if (!ctx->fps.num && ctx->dts==ctx->fps.den)
					ctx->dts = ctx->cur_fps.den;
			}
			ctx->fps = ctx->cur_fps;
		}
		first = GF_FALSE;
		if (is_lhvc) {
			first_lhvc = GF_FALSE;
			if (sps->width > max_ew) max_ew = sps->width;
			if (sps->height > max_eh) max_eh = sps->height;
		} else {
			if (sps->width > max_w) max_w = sps->width;
			if (sps->height > max_h) max_h = sps->height;
		}
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_HEVC_NALU_SEQ_PARAM);
	}

	cfg = ctx->explicit ? lvcc : hvcc;
	count = gf_list_count(ctx->pps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->pps, i);
		layer_id = ((sl->data[0] & 0x1) << 5) | (sl->data[1] >> 3);
		if (!layer_id) *has_hevc_base = GF_TRUE;
		if (!ctx->analyze)
			naludmx_add_param_nalu(layer_id ? lvcc->param_array : cfg->param_array, sl, GF_HEVC_NALU_PIC_PARAM);
	}

	*dsi = *dsi_enh = NULL;
	*dsi_size = *dsi_enh_size = 0;

	if (ctx->explicit || ! (*has_hevc_base) ) {
		naludmx_hevc_set_parall_type(ctx, lvcc);
		gf_odf_hevc_cfg_write(lvcc, dsi, dsi_size);
		*max_width = *max_enh_width = max_ew;
		*max_height = *max_enh_height = max_eh;
	} else {
		naludmx_hevc_set_parall_type(ctx, hvcc);
		gf_odf_hevc_cfg_write(hvcc, dsi, dsi_size);
		if (gf_list_count(lvcc->param_array) ) {
			naludmx_hevc_set_parall_type(ctx, lvcc);
			gf_odf_hevc_cfg_write(lvcc, dsi_enh, dsi_enh_size);
		}
		*max_width = max_w;
		*max_height = max_h;
		*max_enh_width = max_ew;
		*max_enh_height = max_eh;
	}
	count = gf_list_count(hvcc->param_array);
	for (i=0; i<count; i++) {
		GF_NALUFFParamArray *pa = gf_list_get(hvcc->param_array, i);
		gf_list_reset(pa->nalus);
	}
	count = gf_list_count(lvcc->param_array);
	for (i=0; i<count; i++) {
		GF_NALUFFParamArray *pa = gf_list_get(lvcc->param_array, i);
		gf_list_reset(pa->nalus);
	}
	gf_odf_hevc_cfg_del(hvcc);
	gf_odf_hevc_cfg_del(lvcc);
#endif // GPAC_DISABLE_HEVC
}