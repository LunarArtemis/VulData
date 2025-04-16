static s32 gf_vvc_read_sps_bs_internal(GF_BitStream *bs, VVCState *vvc, u8 layer_id, u32 *vui_flag_pos)
{
	s32 vps_id, sps_id;
	u32 i, CtbSizeY;
	VVC_SPS *sps;
	u8 sps_ptl_dpb_hrd_params_present_flag;

	if (vui_flag_pos) *vui_flag_pos = 0;

	sps_id = gf_bs_read_int_log(bs, 4, "sps_id");
	if ((sps_id<0) || (sps_id >= 16)) {
		return -1;
	}
	vps_id = gf_bs_read_int_log(bs, 4, "vps_id");
	if ((vps_id<0) || (vps_id >= 16)) {
		return -1;
	}
	if (!vps_id && !vvc->vps[0].state) {
		vvc->vps[0].state = 1;
		vvc->vps[0].num_ptl = 1;
		vvc->vps[0].max_layers = 1;
		vvc->vps[0].all_layers_independent = 1;
	}

	sps = &vvc->sps[sps_id];
	if (!sps->state) {
		sps->state = 1;
		sps->id = sps_id;
		sps->vps_id = vps_id;
	}
	sps->max_sublayers = 1 + gf_bs_read_int_log(bs, 3, "max_sublayers_minus1");
	sps->chroma_format_idc = gf_bs_read_int_log(bs, 2, "chroma_format_idc");
	sps->log2_ctu_size = 5 + gf_bs_read_int_log(bs, 2, "log2_ctu_size_minus5");
	CtbSizeY = 1<<sps->log2_ctu_size;

	sps_ptl_dpb_hrd_params_present_flag = gf_bs_read_int_log(bs, 1, "sps_ptl_dpb_hrd_params_present_flag");
	if (sps_ptl_dpb_hrd_params_present_flag) {
		VVC_ProfileTierLevel ptl, *p_ptl;
		if (sps->vps_id) {
			p_ptl = &ptl;
		} else {
			p_ptl = &vvc->vps[0].ptl[0];
		}
		memset(p_ptl, 0, sizeof(VVC_ProfileTierLevel));
		p_ptl->pt_present = 1;
		p_ptl->ptl_max_tid = sps->max_sublayers-1;
		vvc_profile_tier_level(bs, p_ptl, 0);
	}
	sps->gdr_enabled = gf_bs_read_int_log(bs, 1, "gdr_enabled");
	sps->ref_pic_resampling = gf_bs_read_int_log(bs, 1, "ref_pic_resampling");
	if (sps->ref_pic_resampling)
		sps->res_change_in_clvs = gf_bs_read_int_log(bs, 1, "res_change_in_clvs");
	sps->width = gf_bs_read_ue_log(bs, "width");
	sps->height = gf_bs_read_ue_log(bs, "height");
	sps->conf_window = gf_bs_read_int_log(bs, 1, "conformance_window_present_flag");
	if (sps->conf_window) {
		u32 SubWidthC, SubHeightC;
		sps->cw_left = gf_bs_read_ue_log(bs, "conformance_window_left");
		sps->cw_right = gf_bs_read_ue_log(bs, "conformance_window_right");
		sps->cw_top = gf_bs_read_ue_log(bs, "conformance_window_top");
		sps->cw_bottom = gf_bs_read_ue_log(bs, "conformance_window_bottom");


		if (sps->chroma_format_idc == 1) {
			SubWidthC = SubHeightC = 2;
		} else if (sps->chroma_format_idc == 2) {
			SubWidthC = 2;
			SubHeightC = 1;
		} else {
			SubWidthC = SubHeightC = 1;
		}
		sps->width -= SubWidthC * (sps->cw_left + sps->cw_right);
		sps->height -= SubHeightC * (sps->cw_top + sps->cw_bottom);
	}
	
	sps->subpic_info_present = gf_bs_read_int_log(bs, 1, "subpic_info_present");
	if (sps->subpic_info_present) {
		sps->nb_subpics = 1 + gf_bs_read_ue_log(bs, "nb_subpics_minus1");
		if (sps->nb_subpics>1) {
			u32 tmpWidthVal, tmpHeightVal;
			sps->independent_subpic_flags = gf_bs_read_int_log(bs, 1, "independent_subpic_flags");
			sps->subpic_same_size = gf_bs_read_int_log(bs, 1, "subpic_same_size");

			tmpWidthVal = (sps->width + CtbSizeY-1) / CtbSizeY;
			tmpWidthVal = gf_get_bit_size(tmpWidthVal);
			tmpHeightVal = (sps->height + CtbSizeY-1) / CtbSizeY;
			tmpHeightVal = gf_get_bit_size(tmpHeightVal);

			for (i=0; i<sps->nb_subpics; i++) {
				if( !sps->subpic_same_size || !i) {
					if (i && (sps->width > CtbSizeY))
						gf_bs_read_int_log(bs, tmpWidthVal, "subpic_ctu_top_left_x");
					if (i && (sps->height > CtbSizeY))
						gf_bs_read_int_log(bs, tmpHeightVal, "subpic_ctu_top_left_y");
					if ((i+1 < sps->nb_subpics) && (sps->width > CtbSizeY))
						gf_bs_read_int_log(bs, tmpWidthVal, "subpic_width_minus1");
					if ((i+1 < sps->nb_subpics) && (sps->height > CtbSizeY))
						gf_bs_read_int_log(bs, tmpHeightVal, "subpic_height_minus1");
				}
				if (!sps->independent_subpic_flags) {
					gf_bs_read_int_log(bs, 1, "subpic_treated_as_pic_flag");
					gf_bs_read_int_log(bs, 1, "loop_filter_across_subpic_enabled_flag");
				}
			}
			sps->subpicid_len = gf_bs_read_ue_log(bs, "subpic_id_len_minus1") + 1;
			sps->subpicid_mapping_explicit = gf_bs_read_int_log(bs, 1, "subpic_id_mapping_explicitly_signalled_flag");
			if (sps->subpicid_mapping_explicit) {
				sps->subpicid_mapping_present = gf_bs_read_int_log(bs, 1, "subpic_id_mapping_present_flag");
				if (sps->subpicid_mapping_present) {
					for (i=0; i<sps->nb_subpics; i++) {
						gf_bs_read_ue_log(bs, "subpic_id");
					}
				}
			}
		}
	} else {
		sps->nb_subpics = 1;
	}
	sps->bitdepth = gf_bs_read_ue_log(bs, "bitdepth_minus8") + 8;
	sps->entropy_coding_sync_enabled_flag = gf_bs_read_int_log(bs, 1, "entropy_coding_sync_enabled_flag");
	sps->entry_point_offsets_present_flag = gf_bs_read_int_log(bs, 1, "entry_point_offsets_present_flag");
	sps->log2_max_poc_lsb = 4 + gf_bs_read_int_log(bs, 4, "log2_max_poc_lsb_minus4");
	if ((sps->poc_msb_cycle_flag = gf_bs_read_int_log(bs, 1, "poc_msb_cycle_flag")))
		sps->poc_msb_cycle_len = 1 + gf_bs_read_ue_log(bs, "poc_msb_cycle_len_minus1");

	u8 sps_num_extra_ph_bits = 8 * gf_bs_read_int_log(bs, 2, "sps_num_extra_ph_bytes");
	for (i=0; i<sps_num_extra_ph_bits; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "extra_ph_bit_present_flag", 1))
			sps->ph_num_extra_bits++;
	}
	u8 sps_num_extra_sh_bits = 8 * gf_bs_read_int_log(bs, 2, "num_extra_sh_bytes");
	for (i=0; i<sps_num_extra_sh_bits; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "extra_sh_bit_present_flag", i))
			sps->sh_num_extra_bits++;
	}

	if (sps_ptl_dpb_hrd_params_present_flag) {
		u8 sps_sublayer_dpb_params_flag = 0;
		if (sps->max_sublayers>1) {
			sps_sublayer_dpb_params_flag = gf_bs_read_int_log(bs, 1, "sps_sublayer_dpb_params_flag");
		}
		for (i=(sps_sublayer_dpb_params_flag ? 0 : sps->max_sublayers-1); i < sps->max_sublayers; i++ ) {
			gf_bs_read_ue_log_idx(bs, "dpb_max_dec_pic_buffering_minus1", i);
			gf_bs_read_ue_log_idx(bs, "dpb_max_num_reorder_pics", i);
			gf_bs_read_ue_log_idx(bs, "dpb_max_latency_increase_plus1", i);
		}
	}
	gf_bs_read_ue_log(bs, "sps_log2_min_luma_coding_block_size_minus2");
	sps->partition_constraints_override_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_partition_constraints_override_enabled_flag");
	gf_bs_read_ue_log(bs, "sps_log2_min_luma_coding_block_size_minus2");
	u8 sps_max_mtt_hierarchy_depth_intra_slice_luma = gf_bs_read_ue_log(bs, "sps_max_mtt_hierarchy_depth_intra_slice_luma");
	if (sps_max_mtt_hierarchy_depth_intra_slice_luma != 0) {
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_bt_min_qt_intra_slice_luma");
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_tt_min_qt_intra_slice_luma");
	}
	u8 sps_qtbtt_dual_tree_intra_flag = 0;
	if (sps->chroma_format_idc) {
		sps_qtbtt_dual_tree_intra_flag = gf_bs_read_int_log(bs, 1, "sps_qtbtt_dual_tree_intra_flag");
	}
	if (sps_qtbtt_dual_tree_intra_flag) {
		gf_bs_read_ue_log(bs, "sps_log2_diff_min_qt_min_cb_intra_slice_chroma");
		u8 sps_max_mtt_hierarchy_depth_intra_slice_chroma = gf_bs_read_ue_log(bs, "sps_max_mtt_hierarchy_depth_intra_slice_chroma");
		if( sps_max_mtt_hierarchy_depth_intra_slice_chroma != 0) {
			gf_bs_read_ue_log(bs, "sps_log2_diff_max_bt_min_qt_intra_slice_chroma");
			gf_bs_read_ue_log(bs, "sps_log2_diff_max_tt_min_qt_intra_slice_chroma");
		}
	}

	gf_bs_read_ue_log(bs, "sps_log2_diff_min_qt_min_cb_inter_slice");
	u8 sps_max_mtt_hierarchy_depth_inter_slice = gf_bs_read_ue_log(bs, "sps_max_mtt_hierarchy_depth_inter_slice");
	if (sps_max_mtt_hierarchy_depth_inter_slice != 0) {
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_bt_min_qt_inter_slice");
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_tt_min_qt_inter_slice");
	}
	u8 max_luma_transform_size_64_flag = 0;
	if (CtbSizeY > 32) {
		max_luma_transform_size_64_flag = gf_bs_read_int_log(bs, 1, "sps_max_luma_transform_size_64_flag");
	}
	sps->transform_skip_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_transform_skip_enabled_flag");

	if (sps->transform_skip_enabled_flag) {
		gf_bs_read_ue_log(bs, "sps_log2_transform_skip_max_size_minus2");
		gf_bs_read_int_log(bs, 1, "sps_bdpcm_enabled_flag");
	}
	if (gf_bs_read_int_log(bs, 1, "sps_mts_enabled_flag")) {
		gf_bs_read_int_log(bs, 1, "sps_explicit_mts_intra_enabled_flag");
		gf_bs_read_int_log(bs, 1, "sps_explicit_mts_inter_enabled_flag");
	}
	Bool lfnst_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_lfnst_enabled_flag");
	sps->joint_cbcr_enabled_flag = 0;
	if (sps->chroma_format_idc) {
		sps->joint_cbcr_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_joint_cbcr_enabled_flag");
		u8 sps_same_qp_table_for_chroma_flag = gf_bs_read_int_log(bs, 1, "sps_same_qp_table_for_chroma_flag");
		u32 numQpTables = sps_same_qp_table_for_chroma_flag ? 1 : (sps->joint_cbcr_enabled_flag ? 3 : 2);
		for (i=0; i<numQpTables; i++) {
			gf_bs_read_se_log_idx(bs, "sps_qp_table_start_minus26", i);
			u32 j, sps_num_points_in_qp_table = 1 + gf_bs_read_ue_log_idx(bs, "sps_num_points_in_qp_table_minus1", i);
			for (j=0; j<sps_num_points_in_qp_table; j++) {
				gf_bs_read_ue_log_idx2(bs, "sps_delta_qp_in_val_minus1", i, j);
				gf_bs_read_ue_log_idx2(bs, "sps_delta_qp_diff_val", i, j);
			}
		}
	}
	sps->sao_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_sao_enabled_flag");
	sps->alf_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_alf_enabled_flag");
	if (sps->alf_enabled_flag && sps->chroma_format_idc) {
		sps->ccalf_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_ccalf_enabled_flag");
	}
	sps->lmcs_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_lmcs_enabled_flag");
	sps->weighted_pred_flag = gf_bs_read_int_log(bs, 1, "sps_weighted_pred_flag");
	sps->weighted_bipred_flag = gf_bs_read_int_log(bs, 1, "sps_weighted_bipred_flag");
	sps->long_term_ref_pics_flag = gf_bs_read_int_log(bs, 1, "sps_long_term_ref_pics_flag");
	if (sps->vps_id>0)
		sps->inter_layer_prediction_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_inter_layer_prediction_enabled_flag");
	sps->idr_rpl_present_flag = gf_bs_read_int_log(bs, 1, "sps_idr_rpl_present_flag");
	u32 sps_rpl1_same_as_rpl0 = gf_bs_read_int_log(bs, 1, "sps_rpl1_same_as_rpl0_flag") ? 1: 2;
	for (i=0; i<sps_rpl1_same_as_rpl0; i++) {
		u32 j;
		sps->num_ref_pic_lists[i] = gf_bs_read_ue_log_idx(bs, "sps_num_ref_pic_lists", i);
		for (j=0; j<sps->num_ref_pic_lists[i]; j++) {
			s32 res = vvc_parse_ref_pic_list_struct(bs, sps, i, j, &sps->rps[i][j]);
			if (res<0) return res;
		}
	}
	gf_bs_read_int_log(bs, 1, "sps_ref_wraparound_enabled_flag");
	sps->temporal_mvp_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_temporal_mvp_enabled_flag");
	if (sps->temporal_mvp_enabled_flag) {
		gf_bs_read_int_log(bs, 1, "sps_sbtmvp_enabled_flag");
	}
	Bool amvr_enabled = gf_bs_read_int_log(bs, 1, "sps_amvr_enabled_flag");
	sps->bdof_control_present_in_ph_flag = 0;
	if (gf_bs_read_int_log(bs, 1, "sps_bdof_enabled_flag")) {
		sps->bdof_control_present_in_ph_flag = gf_bs_read_int_log(bs, 1, "sps_bdof_control_present_in_ph_flag");
	}
	gf_bs_read_int_log(bs, 1, "sps_smvd_enabled_flag");
	sps->dmvr_control_present_in_ph_flag = 0;
	if (gf_bs_read_int_log(bs, 1, "sps_dmvr_enabled_flag")) {
		sps->dmvr_control_present_in_ph_flag = gf_bs_read_int_log(bs, 1, "sps_dmvr_control_present_in_ph_flag");
	}
	sps->mmvd_fullpel_only_enabled_flag = 0;
	if (gf_bs_read_int_log(bs, 1, "sps_mmvd_enabled_flag")) {
		sps->mmvd_fullpel_only_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_mmvd_fullpel_only_enabled_flag");
	}
	u32 MaxNumMergeCand  = 6 - gf_bs_read_ue_log(bs, "sps_six_minus_max_num_merge_cand");

	sps->prof_control_present_in_ph_flag = 0;
	gf_bs_read_int_log(bs, 1, "sps_sbt_enabled_flag");
	if (gf_bs_read_int_log(bs, 1, "sps_affine_enabled_flag")) {
		gf_bs_read_ue_log(bs, "sps_five_minus_max_num_subblock_merge_cand");
		gf_bs_read_int_log(bs, 1, "sps_6param_affine_enabled_flag");
		if (amvr_enabled) {
			gf_bs_read_int_log(bs, 1, "sps_affine_amvr_enabled_flag");
		}
		if (gf_bs_read_int_log(bs, 1, "sps_affine_prof_enabled_flag")) {
			sps->prof_control_present_in_ph_flag = gf_bs_read_int_log(bs, 1, "sps_prof_control_present_in_ph_flag");
		}
	}

	gf_bs_read_int_log(bs, 1, "sps_bcw_enabled_flag");
	gf_bs_read_int_log(bs, 1, "sps_ciip_enabled_flag");
	if (MaxNumMergeCand >= 2) {
		Bool gpm_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_gpm_enabled_flag");
		if (gpm_enabled_flag && (MaxNumMergeCand >= 3)) {
			gf_bs_read_ue_log(bs, "sps_max_num_merge_cand_minus_max_num_gpm_cand");
		}
	}
	gf_bs_read_ue_log(bs, "sps_log2_parallel_merge_level_minus2");

	gf_bs_read_int_log(bs, 1, "sps_isp_enabled_flag");
	gf_bs_read_int_log(bs, 1, "sps_mrl_enabled_flag");
	gf_bs_read_int_log(bs, 1, "sps_mip_enabled_flag");
	if (sps->chroma_format_idc != 0) {
		gf_bs_read_int_log(bs, 1, "sps_cclm_enabled_flag");
	}
	if (sps->chroma_format_idc == 1) {
		gf_bs_read_int_log(bs, 1, "sps_chroma_horizontal_collocated_flag");
		gf_bs_read_int_log(bs, 1, "sps_chroma_vertical_collocated_flag");
	}
	Bool act_enabled_flag = GF_FALSE;
	Bool palette_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_palette_enabled_flag");
	if ((sps->chroma_format_idc == 3) && !max_luma_transform_size_64_flag) {
		act_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_act_enabled_flag");
	}
	if (sps->transform_skip_enabled_flag || palette_enabled_flag) {
		gf_bs_read_ue_log(bs, "sps_min_qp_prime_ts");
	}
	if (gf_bs_read_int_log(bs, 1, "sps_ibc_enabled_flag")) {
		gf_bs_read_ue_log(bs, "sps_six_minus_max_num_ibc_merge_cand");
	}
	if (gf_bs_read_int_log(bs, 1, "sps_ladf_enabled_flag")) {
		u32 num_ladf_intervals_minus2 = gf_bs_read_int_log(bs, 2, "sps_num_ladf_intervals_minus2");
		gf_bs_read_se_log(bs, "sps_ladf_lowest_interval_qp_offset");
		for (i=0; i<num_ladf_intervals_minus2+1; i++) {
			gf_bs_read_se_log_idx(bs, "sps_ladf_qp_offset", i);
			gf_bs_read_ue_log_idx(bs, "sps_ladf_delta_threshold_minus1", i);
		}
	}
	sps->explicit_scaling_list_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_explicit_scaling_list_enabled_flag");
	if (lfnst_enabled_flag && sps->explicit_scaling_list_enabled_flag) {
		gf_bs_read_int_log(bs, 1, "sps_scaling_matrix_for_lfnst_disabled_flag");
	}
	Bool scaling_matrix_for_alternative_colour_space_disabled_flag = 0;
	if (act_enabled_flag && sps->explicit_scaling_list_enabled_flag) {
		scaling_matrix_for_alternative_colour_space_disabled_flag = gf_bs_read_int_log(bs, 1, "sps_scaling_matrix_for_alternative_colour_space_disabled_flag");
	}
	if (scaling_matrix_for_alternative_colour_space_disabled_flag) {
		gf_bs_read_int_log(bs, 1, "sps_scaling_matrix_designated_colour_space_flag");
	}
	sps->dep_quant_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_dep_quant_enabled_flag");
	sps->sign_data_hiding_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_sign_data_hiding_enabled_flag");
	sps->virtual_boundaries_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_virtual_boundaries_enabled_flag");
	if (sps->virtual_boundaries_enabled_flag) {
		sps->virtual_boundaries_present_flag = gf_bs_read_int_log(bs, 1, "sps_virtual_boundaries_present_flag");
		if (sps->virtual_boundaries_present_flag) {
			u32 num_virtual_boundaries = gf_bs_read_ue_log(bs, "sps_num_ver_virtual_boundaries");
			for (i=0; i<num_virtual_boundaries; i++) {
				gf_bs_read_ue_log_idx(bs, "sps_virtual_boundary_pos_x_minus1", i);
			}
			num_virtual_boundaries = gf_bs_read_ue_log(bs, "sps_num_hor_virtual_boundaries");
			for (i=0; i<num_virtual_boundaries; i++) {
				gf_bs_read_ue_log_idx(bs, "sps_virtual_boundary_pos_y_minus1", i);
			}
		}
	}

	if (sps_ptl_dpb_hrd_params_present_flag) {
		if (gf_bs_read_int_log(bs, 1, "sps_timing_hrd_params_present_flag")) {
			Bool general_nal_hrd_params_present_flag, general_vcl_hrd_params_present_flag, general_du_hrd_params_present_flag;
			u32 hrd_cpb_cnt_minus1=0;
			u32 sublayer_cpb_params_present_flag = 0;
			vvc_parse_general_timing_hrd_parameters(bs, sps, NULL, &general_nal_hrd_params_present_flag, &general_vcl_hrd_params_present_flag, &general_du_hrd_params_present_flag, &hrd_cpb_cnt_minus1);
			if (sps->max_sublayers > 1) {
				sublayer_cpb_params_present_flag = gf_bs_read_int_log(bs, 1, "sps_sublayer_cpb_params_present_flag");
			}
			u32 firstSubLayer = sublayer_cpb_params_present_flag ? 0 : sps->max_sublayers - 1;
			vvc_parse_ols_timing_hrd_parameters(bs, firstSubLayer, sps->max_sublayers-1, general_nal_hrd_params_present_flag, general_vcl_hrd_params_present_flag, general_du_hrd_params_present_flag, hrd_cpb_cnt_minus1);

		}
	}

	gf_bs_read_int_log(bs, 1, "sps_field_seq_flag");
	if (vui_flag_pos) {
		*vui_flag_pos = (u32)gf_bs_get_bit_offset(bs);
	}
	//all this to get to VUI !!!
	if (gf_bs_read_int_log(bs, 1, "sps_vui_parameters_present_flag")) {
		gf_bs_read_ue_log(bs, "sps_vui_payload_size_minus1");
		while (!gf_bs_is_align(bs)) {
			gf_bs_read_int_log(bs, 1, "sps_vui_alignment_zero_bit");
		}
		//vui parameters
		Bool vui_progressive_source_flag = gf_bs_read_int_log(bs, 1, "vui_progressive_source_flag");
		Bool vui_interlaced_source_flag = gf_bs_read_int_log(bs, 1, "vui_interlaced_source_flag");
		gf_bs_read_int_log(bs, 1, "vui_non_packed_constraint_flag");
		gf_bs_read_int_log(bs, 1, "vui_non_projected_constraint_flag");
		sps->aspect_ratio_info_present_flag = gf_bs_read_int_log(bs, 1, "vui_aspect_ratio_info_present_flag");
		if (sps->aspect_ratio_info_present_flag) {
			gf_bs_read_int_log(bs, 1, "vui_aspect_ratio_constant_flag");
			sps->sar_idc = gf_bs_read_int_log(bs, 8, "vui_aspect_ratio_idc");
			if (sps->sar_idc== 0xFF) {
				sps->sar_width = gf_bs_read_int_log(bs, 16, "vui_sar_width");
				sps->sar_height = gf_bs_read_int_log(bs, 16, "vui_sar_height");
			}
		}
		sps->overscan_info_present_flag = gf_bs_read_int_log(bs, 1, "vui_overscan_info_present_flag");
		if (sps->overscan_info_present_flag) {
			gf_bs_read_int_log(bs, 1, "vui_overscan_appropriate_flag");
		}
		sps->colour_description_present_flag = gf_bs_read_int_log(bs, 1, "vui_colour_description_present_flag");
		if (sps->colour_description_present_flag) {
			sps->colour_primaries = gf_bs_read_int_log(bs, 8, "vui_colour_primaries");
			sps->transfer_characteristics = gf_bs_read_int_log(bs, 8, "vui_transfer_characteristics");
			sps->matrix_coefficients = gf_bs_read_int_log(bs, 8, "vui_matrix_coeffs");
			sps->video_full_range_flag = gf_bs_read_int_log(bs, 1, "vui_full_range_flag");
		}
		if (gf_bs_read_int_log(bs, 1, " vui_chroma_loc_info_present_flag")) {
			if (vui_progressive_source_flag && !vui_interlaced_source_flag) {
				gf_bs_read_ue_log(bs, "vui_chroma_sample_loc_type_frame");
			} else {
				gf_bs_read_ue_log(bs, "vui_chroma_sample_loc_type_top_field");
				gf_bs_read_ue_log(bs, "vui_chroma_sample_loc_type_bottom_field");
			}
		}
		//WE DON'T PARSE vui_payload_bit_equal_to_one because we dont parse the rest (sps extensions)
		//if needed, see rewrite_vui code
	}
	return sps_id;
}