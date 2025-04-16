static void naludmx_check_pid(GF_Filter *filter, GF_NALUDmxCtx *ctx, Bool force_au_flush)
{
	u32 w, h, ew, eh;
	u8 *dsi, *dsi_enh;
	u32 dsi_size, dsi_enh_size;
	u32 crc_cfg, crc_cfg_enh;
	GF_Fraction sar;
	Bool has_hevc_base = GF_TRUE;
	Bool has_colr_info = GF_FALSE;

	if (ctx->analyze) {
		if (ctx->opid && !ctx->ps_modified) return;
	} else {
		if (!ctx->ps_modified) return;
		if (ctx->opid && (!gf_list_count(ctx->sps) || !gf_list_count(ctx->pps)))
			return;
	}
	ctx->ps_modified = GF_FALSE;

	dsi = dsi_enh = NULL;

	if (ctx->notime) {
		ctx->cur_fps = ctx->fps;
		if (!ctx->cur_fps.num || !ctx->cur_fps.den) {
			ctx->cur_fps.num = 25000;
			ctx->cur_fps.den = 1000;
		}
	}

	if (ctx->codecid==GF_CODECID_HEVC) {
		naludmx_create_hevc_decoder_config(ctx, &dsi, &dsi_size, &dsi_enh, &dsi_enh_size, &w, &h, &ew, &eh, &sar, &has_hevc_base);
	} else if (ctx->codecid==GF_CODECID_VVC) {
		naludmx_create_vvc_decoder_config(ctx, &dsi, &dsi_size, &dsi_enh, &dsi_enh_size, &w, &h, &ew, &eh, &sar, &has_hevc_base);
	} else {
		naludmx_create_avc_decoder_config(ctx, &dsi, &dsi_size, &dsi_enh, &dsi_enh_size, &w, &h, &ew, &eh, &sar);
	}
	crc_cfg = crc_cfg_enh = 0;
	if (dsi) crc_cfg = gf_crc_32(dsi, dsi_size);
	if (dsi_enh) crc_cfg_enh = gf_crc_32(dsi_enh, dsi_enh_size);

	if (!ctx->analyze && (!w || !h)) {
		if (dsi) gf_free(dsi);
		if (dsi_enh) gf_free(dsi_enh);
		return;
	}

	if (!ctx->opid) {
		u32 slice_in_au = ctx->nb_slices_in_au;
		ctx->opid = gf_filter_pid_new(filter);

		naludmx_check_dur(filter, ctx);
		ctx->nb_slices_in_au = slice_in_au;
	}

	if ((ctx->crc_cfg == crc_cfg) && (ctx->crc_cfg_enh == crc_cfg_enh)
		&& (ctx->width==w) && (ctx->height==h)
		&& (ctx->sar.num * sar.den == ctx->sar.den * sar.num)
	) {
		if (dsi) gf_free(dsi);
		if (dsi_enh) gf_free(dsi_enh);
		return;
	}

	if (force_au_flush) {
		naludmx_end_access_unit(ctx);
	}
	naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);
	if (!ctx->analyze && (gf_list_count(ctx->pck_queue)>1))  {
		GF_LOG(dsi_enh ? GF_LOG_DEBUG : GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] xPS changed but could not flush frames before signaling state change %s\n", ctx->log_name, dsi_enh ? "- likely scalable xPS update" : "!"));
	}
	//copy properties at init or reconfig
	gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);

	//don't change codec type if reframing an ES (for HLS SAES)
	if (!ctx->timescale)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, & PROP_UINT(GF_STREAM_VISUAL));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, NULL);
	if (!gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_ID))
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(1));

	ctx->width = w;
	ctx->height = h;
	ctx->sar = sar;
	ctx->crc_cfg = crc_cfg;
	ctx->crc_cfg_enh = crc_cfg_enh;

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, & PROP_UINT( ctx->width));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, & PROP_UINT( ctx->height));
	if (ew && eh) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH_MAX, & PROP_UINT( ew ));
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT_MAX, & PROP_UINT( eh ));
	}
	if (ctx->sar.den)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAR, & PROP_FRAC(ctx->sar));
	else
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAR, NULL);

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_FPS, & PROP_FRAC(ctx->cur_fps));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, & PROP_UINT(ctx->timescale ? ctx->timescale : ctx->cur_fps.num));

	if (ctx->explicit || !has_hevc_base) {
		u32 enh_cid = GF_CODECID_SVC;
		if (ctx->codecid==GF_CODECID_HEVC) enh_cid = GF_CODECID_LHVC;
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT(enh_cid));
		if (dsi) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA_NO_COPY(dsi, dsi_size) );
	} else {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT(ctx->codecid));
		if (dsi) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA_NO_COPY(dsi, dsi_size) );
		if (dsi_enh) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG_ENHANCEMENT, &PROP_DATA_NO_COPY(dsi_enh, dsi_enh_size) );
	}

	if (ctx->bitrate) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_BITRATE, & PROP_UINT(ctx->bitrate));
	}

	if ((ctx->codecid==GF_CODECID_HEVC) && gf_list_count(ctx->vps) ) {
		GF_Err e = naludmx_set_hevc_oinf(ctx, NULL);
		if (e) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[%s] Failed to create OINF chunk\n", ctx->log_name));
		}
		naludmx_set_hevc_linf(ctx);
	}
	if (ctx->duration.num)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

	if (ctx->is_file /* && ctx->index*/) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, & PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD) );
	}
	//set interlaced or remove interlaced property
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_INTERLACED, ctx->interlaced ? & PROP_UINT(GF_TRUE) : NULL);

	if (ctx->codecid==GF_CODECID_HEVC) {
		HEVC_SPS *sps = &ctx->hevc_state->sps[ctx->hevc_state->sps_active_idx];
		if (sps->colour_description_present_flag) {
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_PRIMARIES, & PROP_UINT(sps->colour_primaries) );
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_TRANSFER, & PROP_UINT(sps->transfer_characteristic) );
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_MX, & PROP_UINT(sps->matrix_coeffs) );
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_RANGE, & PROP_BOOL(sps->video_full_range_flag) );
			has_colr_info = GF_TRUE;
		}
	} else if (ctx->codecid==GF_CODECID_VVC) {
	} else {
		/*use the last active SPS*/
		if (ctx->avc_state->sps[ctx->avc_state->sps_active_idx].vui_parameters_present_flag
		&& ctx->avc_state->sps[ctx->avc_state->sps_active_idx].vui.colour_description_present_flag) {
			AVC_VUI *vui = &ctx->avc_state->sps[ctx->avc_state->sps_active_idx].vui;

			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_PRIMARIES, & PROP_UINT(vui->colour_primaries) );
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_TRANSFER, & PROP_UINT(vui->transfer_characteristics) );
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_MX, & PROP_UINT(vui->matrix_coefficients) );
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_RANGE, & PROP_BOOL(vui->video_full_range_flag) );
			has_colr_info = GF_TRUE;
		}
	}

	if (!has_colr_info) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_PRIMARIES, NULL);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_TRANSFER, NULL);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_MX, NULL);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_RANGE, NULL);
	}

	naludmx_update_clli_mdcv(ctx, GF_TRUE);

	naludmx_set_dolby_vision(ctx);

}