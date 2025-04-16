static void naludmx_create_vvc_decoder_config(GF_NALUDmxCtx *ctx, u8 **dsi, u32 *dsi_size, u8 **dsi_enh, u32 *dsi_enh_size, u32 *max_width, u32 *max_height, u32 *max_enh_width, u32 *max_enh_height, GF_Fraction *sar, Bool *has_vvc_base)
{
	u32 i, count;
	u8 layer_id;
	Bool first = GF_TRUE;
	Bool first_lvvc = GF_TRUE;
	GF_VVCConfig *cfg;
	u32 max_w, max_h, max_ew, max_eh;

	*has_vvc_base = GF_FALSE;

	max_w = max_h = 0;
	max_ew = max_eh = 0;
	sar->num = sar->den = 0;

	cfg = gf_odf_vvc_cfg_new();
	cfg->nal_unit_size = ctx->nal_length;

	//check we have one pps or sps in base layer
	count = gf_list_count(ctx->sps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->sps, i);
		layer_id = (sl->data[0] & 0x3f);
		//todo, base is not always 0 !
		if (!layer_id) {
			*has_vvc_base = GF_TRUE;
			break;
		}
	}
	count = gf_list_count(ctx->pps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->pps, i);
		layer_id = (sl->data[0] & 0x3f);
		//todo, base is not always 0 !
		if (!layer_id) {
			*has_vvc_base = GF_TRUE;
			break;
		}
	}
	//assign vps first so that they are serialized first
	count = gf_list_count(ctx->vps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->vps, i);
		VVC_VPS *vps = &ctx->vvc_state->vps[sl->id];

		if (!i) {
			cfg->avgFrameRate = vps->rates[0].avg_pic_rate;
			cfg->constantFrameRate = vps->rates[0].constant_pic_rate_idc;
			cfg->numTemporalLayers = vps->max_sub_layers;
		}
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_VVC_NALU_VID_PARAM);
	}

	count = gf_list_count(ctx->sps);
	for (i=0; i<count; i++) {
		Bool is_lvvc = GF_FALSE;
		GF_NALUFFParam *sl = gf_list_get(ctx->sps, i);
		VVC_SPS *sps = &ctx->vvc_state->sps[sl->id];
		layer_id = sl->data[0] & 0x3f;
		if (!layer_id) *has_vvc_base = GF_TRUE;

		if (ctx->explicit || layer_id) {
			is_lvvc = GF_TRUE;
		}

		if (first || (is_lvvc && first_lvvc) ) {
			VVC_VPS *vps = &ctx->vvc_state->vps[sps->vps_id];
			cfg->avgFrameRate = 0;
			cfg->constantFrameRate = 1;
			cfg->numTemporalLayers = sps->max_sublayers;
			cfg->nal_unit_size = ctx->nal_length;
			cfg->ptl_present = vps->num_ptl ? 1 : 0;

			if (vps->num_ptl) {
				cfg->num_constraint_info = vps->ptl[0].gci_present ? 1 : 12;
				cfg->general_profile_idc = vps->ptl[0].general_profile_idc;
				cfg->general_tier_flag = vps->ptl[0].general_tier_flag;
				cfg->general_level_idc = vps->ptl[0].general_level_idc;
				cfg->ptl_frame_only_constraint = vps->ptl[0].frame_only_constraint;
				cfg->ptl_multilayer_enabled = vps->ptl[0].multilayer_enabled;

				cfg->general_constraint_info = gf_malloc(sizeof(u8) * cfg-> num_constraint_info);
				if (cfg->general_constraint_info)
					memcpy(cfg->general_constraint_info, vps->ptl[0].gci, cfg->num_constraint_info);

				//todo set temporal sublayers
				cfg->ptl_sublayer_present_mask = 0;
				cfg->num_sub_profiles = 0;
				cfg->ols_idx = 0;
			}
			cfg->chroma_format = sps->chroma_format_idc;
			cfg->bit_depth = sps->bitdepth;
			cfg->maxPictureWidth = sps->width;
			cfg->maxPictureHeight = sps->height;

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
				gf_media_get_reduced_frame_rate(&ctx->cur_fps.num, &ctx->cur_fps.den);

				if (!ctx->fps.num && ctx->dts==ctx->fps.den)
					ctx->dts = ctx->cur_fps.den;
			}
			ctx->fps = ctx->cur_fps;
		}
		first = GF_FALSE;
		if (is_lvvc) {
			first_lvvc = GF_FALSE;
			if (sps->width > max_ew) max_ew = sps->width;
			if (sps->height > max_eh) max_eh = sps->height;
		} else {
			if (sps->width > max_w) max_w = sps->width;
			if (sps->height > max_h) max_h = sps->height;
		}
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_VVC_NALU_SEQ_PARAM);
	}

	count = gf_list_count(ctx->pps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->pps, i);
		layer_id = sl->data[0] & 0x3F;
		if (!layer_id) *has_vvc_base = GF_TRUE;
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_VVC_NALU_PIC_PARAM);
	}

	count = gf_list_count(ctx->vvc_dci);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->vvc_dci, i);
		layer_id = sl->data[0] & 0x3F;
		if (!layer_id) *has_vvc_base = GF_TRUE;
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_VVC_NALU_DEC_PARAM);
	}

	count = gf_list_count(ctx->vvc_opi);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->vvc_opi, i);
		layer_id = sl->data[0] & 0x3F;
		if (!layer_id) *has_vvc_base = GF_TRUE;
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_VVC_NALU_OPI);
	}

	count = gf_list_count(ctx->vvc_aps_pre);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->vvc_aps_pre, i);
		layer_id = sl->data[0] & 0x3F;
		if (!layer_id) *has_vvc_base = GF_TRUE;
		if (!ctx->analyze)
			naludmx_add_param_nalu(cfg->param_array, sl, GF_VVC_NALU_APS_PREFIX);
	}

	*dsi = *dsi_enh = NULL;
	*dsi_size = *dsi_enh_size = 0;

	gf_odf_vvc_cfg_write(cfg, dsi, dsi_size);
	*max_width = max_w;
	*max_height = max_h;
	*max_enh_width = max_ew;
	*max_enh_height = max_eh;

	count = gf_list_count(cfg->param_array);
	for (i=0; i<count; i++) {
		GF_NALUFFParamArray *pa = gf_list_get(cfg->param_array, i);
		gf_list_reset(pa->nalus);
	}
	gf_odf_vvc_cfg_del(cfg);
}