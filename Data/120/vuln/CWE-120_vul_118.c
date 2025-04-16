static void naludmx_queue_param_set(GF_NALUDmxCtx *ctx, char *data, u32 size, u32 ps_type, s32 ps_id, u32 tid, u32 lid)
{
	GF_List *list = NULL, *alt_list = NULL;
	GF_NALUFFParam *sl;
	u32 i, count, crc;
	Bool flush_au = GF_FALSE;

	if (!size) return;
	crc = gf_crc_32(data, size);

	if (ctx->codecid==GF_CODECID_HEVC) {
		switch (ps_type) {
		case GF_HEVC_NALU_VID_PARAM:
			if (!ctx->vps) ctx->vps = gf_list_new();
			list = ctx->vps;
			flush_au = GF_TRUE;
			break;
		case GF_HEVC_NALU_SEQ_PARAM:
			list = ctx->sps;
			flush_au = GF_TRUE;
			break;
		case GF_HEVC_NALU_PIC_PARAM:
			list = ctx->pps;
			break;
		default:
			assert(0);
			return;
		}
	} else if (ctx->codecid==GF_CODECID_VVC) {
		switch (ps_type) {
		case GF_VVC_NALU_VID_PARAM:
			if (!ctx->vps) ctx->vps = gf_list_new();
			list = ctx->vps;
			flush_au = GF_TRUE;
			break;
		case GF_VVC_NALU_SEQ_PARAM:
			list = ctx->sps;
			flush_au = GF_TRUE;
			break;
		case GF_VVC_NALU_PIC_PARAM:
			list = ctx->pps;
			break;
		case GF_VVC_NALU_DEC_PARAM:
			if (!ctx->vvc_dci) ctx->vvc_dci = gf_list_new();
			list = ctx->vvc_dci;
			break;
		case GF_VVC_NALU_OPI:
			if (!ctx->vvc_opi) ctx->vvc_opi = gf_list_new();
			list = ctx->vvc_opi;
			break;
		case GF_VVC_NALU_APS_PREFIX:
			if (!ctx->vvc_aps_pre) ctx->vvc_aps_pre = gf_list_new();
			list = ctx->vvc_aps_pre;
			break;
		default:
			assert(0);
			return;
		}
	} else {
		switch (ps_type) {
		case GF_AVC_NALU_SEQ_PARAM:
			flush_au = GF_TRUE;
		case GF_AVC_NALU_SVC_SUBSEQ_PARAM:
			list = ctx->sps;
			break;
		case GF_AVC_NALU_PIC_PARAM:
			list = ctx->pps;
			alt_list = ctx->pps_svc;
			break;
		case GF_AVC_NALU_SEQ_PARAM_EXT:
			if (!ctx->sps_ext) ctx->sps_ext = gf_list_new();
			list = ctx->sps_ext;
			break;
		default:
			assert(0);
			return;
		}
	}
	sl = NULL;
	count = gf_list_count(list);
	for (i=0; i<count; i++) {
		sl = gf_list_get(list, i);
		if (sl->id != ps_id) {
			sl = NULL;
			continue;
		}
		//same ID, same CRC, we don't change our state
		if (sl->crc == crc) return;
		break;
	}
	//handle alt PPS list for SVC
	if (!sl && alt_list) {
		count = gf_list_count(alt_list);
		for (i=0; i<count; i++) {
			sl = gf_list_get(alt_list, i);
			if (sl->id != ps_id) {
				sl = NULL;
				continue;
			}
			//same ID, same CRC, we don't change our state
			if (sl->crc == crc) return;
			break;
		}
	}

	if (lid || tid) flush_au = GF_FALSE;

	if (sl) {
		//otherwise we keep this new param set
		sl->data = gf_realloc(sl->data, size);
		memcpy(sl->data, data, size);
		sl->size = size;
		sl->crc = crc;
		ctx->ps_modified = GF_TRUE;
		//flush AU if we have a slice
		if (ctx->opid && flush_au && ctx->first_pck_in_au && ctx->nb_slices_in_au) {
			naludmx_end_access_unit(ctx);
		}
		return;
	}
	//TODO we might want to purge the list after a while !!

	GF_SAFEALLOC(sl, GF_NALUFFParam);
	if (!sl) return;
	sl->data = gf_malloc(sizeof(char) * size);
	if (!sl->data) {
		gf_free(sl);
		return;
	}
	memcpy(sl->data, data, size);
	sl->size = size;
	sl->id = ps_id;
	sl->crc = crc;

	ctx->ps_modified = GF_TRUE;
	//flush AU if we have a slice
	if (ctx->opid && flush_au && ctx->first_pck_in_au && ctx->nb_slices_in_au) {
		naludmx_end_access_unit(ctx);
	}
	gf_list_add(list, sl);
}