static s32 gf_vvc_read_pps_bs_internal(GF_BitStream *bs, VVCState *vvc)
{
	u32 i;
	s32 pps_id;
	VVC_PPS *pps;

	//NAL header already read
	pps_id = gf_bs_read_int_log(bs, 6, "pps_id");

	if ((pps_id < 0) || (pps_id >= 64)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] wrong PPS ID %d in PPS\n", pps_id));
		return -1;
	}
	pps = &vvc->pps[pps_id];

	if (!pps->state) {
		pps->id = pps_id;
		pps->state = 1;
	}
	pps->sps_id = gf_bs_read_int_log(bs, 4, "sps_id");
	if (((s32)pps->sps_id<0) || (pps->sps_id >= 16)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] wrong SPS ID %d in PPS\n", pps->sps_id));
		pps->sps_id=0;
		return -1;
	}
	vvc->sps_active_idx = pps->sps_id; /*set active sps*/
	pps->mixed_nal_types = gf_bs_read_int_log(bs, 1, "mixed_nal_types");
	pps->width = gf_bs_read_ue_log(bs, "width");
	pps->height = gf_bs_read_ue_log(bs, "height");
	pps->conf_window = gf_bs_read_int_log(bs, 1, "conformance_window_flag");
	if (pps->conf_window) {
		pps->cw_left = gf_bs_read_ue_log(bs, "conf_win_left_offset");
		pps->cw_right = gf_bs_read_ue_log(bs, "conf_win_right_offset");
		pps->cw_top = gf_bs_read_ue_log(bs, "conf_win_top_offset");
		pps->cw_bottom = gf_bs_read_ue_log(bs, "conf_win_bottom_offset");
	}
	//scaling window
	if (gf_bs_read_int_log(bs, 1, "scaling_window_explicit_signaling_flag")) {
		gf_bs_read_se_log(bs, "scaling_win_left_offset");
		gf_bs_read_se_log(bs, "scaling_win_right_offset");
		gf_bs_read_se_log(bs, "scaling_win_top_offset");
		gf_bs_read_se_log(bs, "scaling_win_bottom_offset");
	}
	pps->output_flag_present_flag = gf_bs_read_int_log(bs, 1, "output_flag_present_flag");
	pps->no_pic_partition_flag = gf_bs_read_int_log(bs, 1, "no_pic_partition_flag");
	pps->subpic_id_mapping_present_flag = gf_bs_read_int_log(bs, 1, "subpic_id_mapping_present_flag");
	u32 pps_num_subpics = 1;
	if (pps->subpic_id_mapping_present_flag) {
		u32 pps_subpic_id_len;
		if (!pps->no_pic_partition_flag) {
			pps_num_subpics = 1+gf_bs_read_ue_log(bs, "pps_num_subpics_minus1");
		}
		pps_subpic_id_len = 1 + gf_bs_read_ue(bs);
		for (i=0; i<pps_num_subpics; i++) {
			gf_bs_read_int_log_idx(bs, pps_subpic_id_len, "subpic_id", i);
		}
	}
	pps->single_slice_per_subpic_flag = 1;
	pps->num_slices_in_pic = 1;

	pps->num_tiles_in_pic = 0;
	if (!pps->no_pic_partition_flag) {
		u32 ctu_size = 5 + gf_bs_read_int_log(bs, 2, "pps_log2_ctu_size_minus5");
		u32 num_exp_tile_columns = 1 + gf_bs_read_ue_log(bs, "num_exp_tile_columns_minus1");
		u32 num_exp_tile_rows = 1 + gf_bs_read_ue_log(bs, "num_exp_tile_rows_minus1");

		if (num_exp_tile_columns > VVC_MAX_TILE_COLS) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] wrong num tile columns %d in PPS\n", num_exp_tile_columns));
			pps->sps_id=0;
			return -1;
		}
		if (num_exp_tile_rows > VVC_MAX_TILE_ROWS) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] wrong num tile rows %d in PPS\n", num_exp_tile_rows));
			pps->sps_id=0;
			return -1;
		}

		ctu_size = 1<<ctu_size;
		pps->pic_width_in_ctbsY = pps->width / ctu_size;
		if (pps->pic_width_in_ctbsY * ctu_size < pps->width) pps->pic_width_in_ctbsY++;
		pps->pic_height_in_ctbsY = pps->height / ctu_size;
		if (pps->pic_height_in_ctbsY * ctu_size < pps->height) pps->pic_height_in_ctbsY++;

		u32 nb_ctb_left = pps->pic_width_in_ctbsY;
		pps->num_tile_cols=0;
		u32 nb_ctb_last=0;
		for (i=0; i<num_exp_tile_columns; i++) {
			u32 nb_ctb_width = 1 + gf_bs_read_ue_log_idx(bs, "tile_column_width_minus1", i);
			if (nb_ctb_left < nb_ctb_width) {
				pps->sps_id=0;
				return -1;
			}
			nb_ctb_left -= nb_ctb_width;
			pps->tile_cols_width_ctb[i] = nb_ctb_width;
			nb_ctb_last = nb_ctb_width;
			pps->num_tile_cols++;
			if (pps->num_tile_cols > VVC_MAX_TILE_COLS) {
				pps->sps_id=0;
				return -1;
			}
		}
		u32 uni_size_ctb = nb_ctb_last;
		while (nb_ctb_left >= uni_size_ctb) {
			nb_ctb_left -= uni_size_ctb;
			if (pps->num_tile_cols >= VVC_MAX_TILE_COLS) {
				pps->sps_id=0;
				return -1;
			}
			pps->tile_cols_width_ctb[pps->num_tile_cols] = uni_size_ctb;
			pps->num_tile_cols++;
		}
		if (nb_ctb_left>0) {
			if (pps->num_tile_cols >= VVC_MAX_TILE_COLS) {
				pps->sps_id=0;
				return -1;
			}
			pps->tile_cols_width_ctb[pps->num_tile_cols] = nb_ctb_left;
			pps->num_tile_cols++;
		}

		nb_ctb_left = pps->pic_height_in_ctbsY;
		nb_ctb_last=0;
		pps->num_tile_rows=0;
		for (i=0; i<num_exp_tile_rows; i++) {
			u32 nb_ctb_height = 1 + gf_bs_read_ue_log_idx(bs, "tile_row_height_minus1", i);
			if (nb_ctb_left < nb_ctb_height) {
				pps->sps_id=0;
				return -1;
			}
			nb_ctb_left -= nb_ctb_height;
			pps->tile_rows_height_ctb[i] = nb_ctb_height;
			pps->num_tile_rows++;
			nb_ctb_last = nb_ctb_height;
			if (pps->num_tile_rows > VVC_MAX_TILE_ROWS) {
				pps->sps_id=0;
				return -1;
			}
		}
		uni_size_ctb = nb_ctb_last;
		while (nb_ctb_left >= uni_size_ctb) {
			nb_ctb_left -= uni_size_ctb;
			if (pps->num_tile_rows >= VVC_MAX_TILE_ROWS) {
				pps->sps_id=0;
				return -1;
			}
			pps->tile_rows_height_ctb[pps->num_tile_rows] = uni_size_ctb;
			pps->num_tile_rows++;
		}
		if (nb_ctb_left>0) {
			if (pps->num_tile_rows >= VVC_MAX_TILE_ROWS) {
				pps->sps_id=0;
				return -1;
			}
			pps->tile_rows_height_ctb[pps->num_tile_rows] = nb_ctb_left;
			pps->num_tile_rows++;
		}

		pps->num_tiles_in_pic = pps->num_tile_cols * pps->num_tile_rows;
		pps->slice_address_len = gf_get_bit_size(pps->num_tiles_in_pic);
		if (pps->num_tiles_in_pic > 1) {
			gf_bs_read_int_log(bs, 1, "pps_loop_filter_across_tiles_enabled_flag");
			pps->rect_slice_flag = gf_bs_read_int_log(bs, 1, "pps_rect_slice_flag");
		}

		if (pps->rect_slice_flag) {
			pps->single_slice_per_subpic_flag = gf_bs_read_int_log(bs, 1, "pps_single_slice_per_subpic_flag");
			pps->num_slices_in_pic = pps_num_subpics;
		}

		if (pps->rect_slice_flag && !pps->single_slice_per_subpic_flag) {
			pps->num_slices_in_pic = 1 + gf_bs_read_ue_log(bs, "pps_num_slices_in_pic_minus1");
			u8 tile_idx_delta_present_flag = 0;
			if (pps->num_slices_in_pic > 1) {
				tile_idx_delta_present_flag = gf_bs_read_int_log(bs, 1, "pps_tile_idx_delta_present_flag");
			}
			for (i=0; i<pps->num_slices_in_pic-1; i++) {
				//TODO FIXME we assume single slice per tile
				u32 SliceTopLeftTileIdx=0;
				u32 RowHeightVal=0;


				u32 slice_width_in_tiles_minus1=0;
				u32 slice_height_in_tiles_minus1=0;
				if (SliceTopLeftTileIdx % pps->num_tile_cols != pps->num_tile_cols - 1) {
					slice_width_in_tiles_minus1 = gf_bs_read_ue_log_idx(bs, "pps_slice_width_in_tiles_minus1", i);
				}

				if ( (SliceTopLeftTileIdx / pps->num_tile_cols != pps->num_tile_rows - 1)
					&& (tile_idx_delta_present_flag || (SliceTopLeftTileIdx % pps->num_tile_cols == 0) )
				) {
					slice_height_in_tiles_minus1 = gf_bs_read_ue_log_idx(bs, "pps_slice_height_in_tiles_minus1", i);
				}

				if (!slice_width_in_tiles_minus1 && !slice_height_in_tiles_minus1 && (RowHeightVal > 1 )
				) {
					u32 j, num_exp_slices_in_tile = gf_bs_read_ue_log_idx(bs, "pps_num_exp_slices_in_tile", i);
					for (j=0; j<num_exp_slices_in_tile; j++) {
						gf_bs_read_ue_log_idx2 (bs, "pps_exp_slice_height_in_ctus_minus1", i, j);
					}
					//TODO FIXME i += NumSlicesInTile[ i ] − 1
				}

				if (tile_idx_delta_present_flag && (i < pps->num_slices_in_pic)) {
					gf_bs_read_se_log_idx(bs, "pps_tile_idx_delta_val", i);
				}

			}
		}
		if (!pps->rect_slice_flag || pps->single_slice_per_subpic_flag || (pps->num_slices_in_pic > 1)) {
			gf_bs_read_int_log(bs, 1, "pps_loop_filter_across_slices_enabled_flag");
		}
	}


	pps->cabac_init_present_flag = gf_bs_read_int_log(bs, 1, "pps_cabac_init_present_flag");
	for (i=0; i<2; i++) {
		pps->num_ref_idx_default_active[i] = 1 + gf_bs_read_ue_log_idx(bs, "pps_num_ref_idx_default_active_minus1", i);
	}
	pps->rpl1_idx_present_flag = gf_bs_read_int_log(bs, 1, "pps_rpl1_idx_present_flag");
	pps->weighted_pred_flag = gf_bs_read_int_log(bs, 1, "pps_weighted_pred_flag");
	pps->weighted_bipred_flag = gf_bs_read_int_log(bs, 1, "pps_weighted_bipred_flag");
	if (gf_bs_read_int_log(bs, 1, "pps_ref_wraparound_enabled_flag")) {
		gf_bs_read_ue_log(bs, "pps_pic_width_minus_wraparound_offset");
	}
	gf_bs_read_se_log(bs, "pps_init_qp_minus26");
	pps->cu_qp_delta_enabled_flag = gf_bs_read_int_log(bs, 1, "pps_cu_qp_delta_enabled_flag");
	pps->slice_chroma_qp_offsets_present_flag = 0;
	pps->chroma_tool_offsets_present_flag = gf_bs_read_int_log(bs, 1, "pps_chroma_tool_offsets_present_flag");
	if (pps->chroma_tool_offsets_present_flag) {
		gf_bs_read_se_log(bs, "pps_cb_qp_offset");
		gf_bs_read_se_log(bs, "pps_cr_qp_offset");
		u8 joint_cbcr_qp_offset_present_flag = gf_bs_read_int_log(bs, 1, "pps_joint_cbcr_qp_offset_present_flag");
		if (joint_cbcr_qp_offset_present_flag) {
			gf_bs_read_se_log(bs, "pps_joint_cbcr_qp_offset_value");
		}
		pps->slice_chroma_qp_offsets_present_flag = gf_bs_read_int_log(bs, 1, "pps_slice_chroma_qp_offsets_present_flag");
		pps->cu_chroma_qp_offset_list_enabled_flag = gf_bs_read_int_log(bs, 1, "pps_cu_chroma_qp_offset_list_enabled_flag");
		if (pps->cu_chroma_qp_offset_list_enabled_flag) {
			u32 pps_chroma_qp_offset_list_len = 1 + gf_bs_read_ue_log(bs, "pps_chroma_qp_offset_list_len_minus1");
			for (i=0; i<pps_chroma_qp_offset_list_len; i++) {
				gf_bs_read_se_log_idx(bs, "pps_cb_qp_offset_list", i);
				gf_bs_read_se_log_idx(bs, "pps_cr_qp_offset_list", i);
				if (joint_cbcr_qp_offset_present_flag) {
					gf_bs_read_se_log_idx(bs, "pps_joint_cbcr_qp_offset_list", i);
				}
			}
		}
	}

	pps->dbf_info_in_ph_flag = 0;
	pps->deblocking_filter_disabled_flag = 0;
	pps->deblocking_filter_override_enabled_flag = 0;
	if (gf_bs_read_int_log(bs, 1, "pps_deblocking_filter_control_present_flag")) {
		pps->deblocking_filter_override_enabled_flag = gf_bs_read_int_log(bs, 1, "pps_deblocking_filter_override_enabled_flag");
		pps->deblocking_filter_disabled_flag = gf_bs_read_int_log(bs, 1, "pps_deblocking_filter_disabled_flag");

		if (!pps->no_pic_partition_flag && pps->deblocking_filter_override_enabled_flag) {
			pps->dbf_info_in_ph_flag = gf_bs_read_int_log(bs, 1, "pps_dbf_info_in_ph_flag");
		}
		if (!pps->deblocking_filter_disabled_flag) {
			gf_bs_read_se_log(bs, "pps_luma_beta_offset_div2");
			gf_bs_read_se_log(bs, "pps_luma_tc_offset_div2");
			if (pps->chroma_tool_offsets_present_flag) {
				gf_bs_read_se_log(bs, "pps_cb_beta_offset_div2");
				gf_bs_read_se_log(bs, "pps_cb_tc_offset_div2");
				gf_bs_read_se_log(bs, "pps_cr_beta_offset_div2");
				gf_bs_read_se_log(bs, "pps_cr_tc_offset_div2");
			}
		}
	}
	pps->wp_info_in_ph_flag = 0;
	pps->qp_delta_info_in_ph_flag = 0;
	pps->sao_info_in_ph_flag = 0;
	if (!pps->no_pic_partition_flag) {
		pps->rpl_info_in_ph_flag = gf_bs_read_int_log(bs, 1, "pps_rpl_info_in_ph_flag");
		pps->sao_info_in_ph_flag = gf_bs_read_int_log(bs, 1, "pps_sao_info_in_ph_flag");
		pps->alf_info_in_ph_flag = gf_bs_read_int_log(bs, 1, "pps_alf_info_in_ph_flag");
		if ( (pps->weighted_pred_flag || pps->weighted_bipred_flag) && pps->rpl_info_in_ph_flag) {
			pps->wp_info_in_ph_flag = gf_bs_read_int_log(bs, 1, "pps_wp_info_in_ph_flag");
		}
		pps->qp_delta_info_in_ph_flag = gf_bs_read_int_log(bs, 1, "pps_qp_delta_info_in_ph_flag");
	}
	pps->picture_header_extension_present_flag = gf_bs_read_int_log(bs, 1, "pps_picture_header_extension_present_flag");
	pps->slice_header_extension_present_flag = gf_bs_read_int_log(bs, 1, "pps_slice_header_extension_present_flag");

	//TODO
	if (gf_bs_read_int_log(bs, 1, "pps_extension_flag")) {
		//while ( more_rbsp_data()) bit(1);
	}
	//rbsp_trailing_bits()
	return pps_id;
}