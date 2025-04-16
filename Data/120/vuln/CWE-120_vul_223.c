void slice_segment_header::dump_slice_segment_header(const decoder_context* ctx, int fd) const
{
  FILE* fh;
  if (fd==1) fh=stdout;
  else if (fd==2) fh=stderr;
  else { return; }

#define LOG0(t) log2fh(fh, t)
#define LOG1(t,d) log2fh(fh, t,d)
#define LOG2(t,d1,d2) log2fh(fh, t,d1,d2)
#define LOG3(t,d1,d2,d3) log2fh(fh, t,d1,d2,d3)
#define LOG4(t,d1,d2,d3,d4) log2fh(fh, t,d1,d2,d3,d4)

  const pic_parameter_set* pps = ctx->get_pps(slice_pic_parameter_set_id);
  assert(pps->pps_read); // TODO: error handling

  const seq_parameter_set* sps = ctx->get_sps((int)pps->seq_parameter_set_id);
  assert(sps->sps_read); // TODO: error handling


  LOG0("----------------- SLICE -----------------\n");
  LOG1("first_slice_segment_in_pic_flag      : %d\n", first_slice_segment_in_pic_flag);
  if (ctx->get_nal_unit_type() >= NAL_UNIT_BLA_W_LP &&
      ctx->get_nal_unit_type() <= NAL_UNIT_RESERVED_IRAP_VCL23) {
    LOG1("no_output_of_prior_pics_flag         : %d\n", no_output_of_prior_pics_flag);
  }

  LOG1("slice_pic_parameter_set_id           : %d\n", slice_pic_parameter_set_id);

  if (!first_slice_segment_in_pic_flag) {
    //if (pps->dependent_slice_segments_enabled_flag) {
      LOG1("dependent_slice_segment_flag         : %d\n", dependent_slice_segment_flag);
      //}
    LOG1("slice_segment_address                : %d\n", slice_segment_address);
  }

  //if (!dependent_slice_segment_flag)
    {
    //for (int i=0; i<pps->num_extra_slice_header_bits; i++) {
    //slice_reserved_flag[i]

    LOG1("slice_type                           : %c\n",
         slice_type == 0 ? 'B' :
         slice_type == 1 ? 'P' : 'I');

    if (pps->output_flag_present_flag) {
      LOG1("pic_output_flag                      : %d\n", pic_output_flag);
    }

    if (sps->separate_colour_plane_flag == 1) {
      LOG1("colour_plane_id                      : %d\n", colour_plane_id);
    }

    LOG1("slice_pic_order_cnt_lsb              : %d\n", slice_pic_order_cnt_lsb);

    if (ctx->get_nal_unit_type() != NAL_UNIT_IDR_W_RADL &&
        ctx->get_nal_unit_type() != NAL_UNIT_IDR_N_LP) {
      LOG1("short_term_ref_pic_set_sps_flag      : %d\n", short_term_ref_pic_set_sps_flag);

      if (!short_term_ref_pic_set_sps_flag) {
        LOG1("ref_pic_set[ %2d ]: ",sps->num_short_term_ref_pic_sets());
        dump_compact_short_term_ref_pic_set(&slice_ref_pic_set, 16, fh);
      }
      else if (sps->num_short_term_ref_pic_sets() > 1) {
        LOG1("short_term_ref_pic_set_idx           : %d\n", short_term_ref_pic_set_idx);
        dump_compact_short_term_ref_pic_set(&sps->ref_pic_sets[short_term_ref_pic_set_idx], 16, fh);
      }

      if (sps->long_term_ref_pics_present_flag) {
        if (sps->num_long_term_ref_pics_sps > 0) {
          LOG1("num_long_term_sps                        : %d\n", num_long_term_sps);
        }

        LOG1("num_long_term_pics                       : %d\n", num_long_term_pics);

#if 0
        for (int i=0; i<num_long_term_sps + num_long_term_pics; i++) {
          LOG2("PocLsbLt[%d]            : %d\n", i, ctx->PocLsbLt[i]);
          LOG2("UsedByCurrPicLt[%d]     : %d\n", i, ctx->UsedByCurrPicLt[i]);
          LOG2("DeltaPocMsbCycleLt[%d]  : %d\n", i, ctx->DeltaPocMsbCycleLt[i]);
        }
#endif
      }

      if (sps->sps_temporal_mvp_enabled_flag) {
        LOG1("slice_temporal_mvp_enabled_flag : %d\n", slice_temporal_mvp_enabled_flag);
      }
    }


    if (sps->sample_adaptive_offset_enabled_flag) {
      LOG1("slice_sao_luma_flag             : %d\n", slice_sao_luma_flag);
      LOG1("slice_sao_chroma_flag           : %d\n", slice_sao_chroma_flag);
    }


    if (slice_type == SLICE_TYPE_P || slice_type == SLICE_TYPE_B) {
      LOG1("num_ref_idx_active_override_flag : %d\n", num_ref_idx_active_override_flag);

      LOG2("num_ref_idx_l0_active          : %d %s\n", num_ref_idx_l0_active,
           num_ref_idx_active_override_flag ? "" : "(from PPS)");

      if (slice_type == SLICE_TYPE_B) {
        LOG2("num_ref_idx_l1_active          : %d %s\n", num_ref_idx_l1_active,
             num_ref_idx_active_override_flag ? "" : "(from PPS)");
      }

      if (pps->lists_modification_present_flag && NumPocTotalCurr > 1)
        {
          LOG1("ref_pic_list_modification_flag_l0 : %d\n", ref_pic_list_modification_flag_l0);
          if (ref_pic_list_modification_flag_l0) {
            for (int i=0;i<num_ref_idx_l0_active;i++) {
              LOG2("  %d: %d\n",i,list_entry_l0[i]);
            }
          }

          LOG1("ref_pic_list_modification_flag_l1 : %d\n", ref_pic_list_modification_flag_l1);
          if (ref_pic_list_modification_flag_l1) {
            for (int i=0;i<num_ref_idx_l1_active;i++) {
              LOG2("  %d: %d\n",i,list_entry_l1[i]);
            }
          }
        }

      if (slice_type == SLICE_TYPE_B) {
        LOG1("mvd_l1_zero_flag               : %d\n", mvd_l1_zero_flag);
      }

      LOG1("cabac_init_flag                : %d\n", cabac_init_flag);

      if (slice_temporal_mvp_enabled_flag) {
        LOG1("collocated_from_l0_flag        : %d\n", collocated_from_l0_flag);
        LOG1("collocated_ref_idx             : %d\n", collocated_ref_idx);
      }

      if ((pps->weighted_pred_flag   && slice_type == SLICE_TYPE_P) ||
          (pps->weighted_bipred_flag && slice_type == SLICE_TYPE_B))
        {
          LOG1("luma_log2_weight_denom         : %d\n", luma_log2_weight_denom);
          if (sps->chroma_format_idc != 0) {
            LOG1("ChromaLog2WeightDenom          : %d\n", ChromaLog2WeightDenom);
          }

          for (int l=0;l<=1;l++)
            if (l==0 || (l==1 && slice_type == SLICE_TYPE_B))
              {
                int num_ref = (l==0 ?
                               num_ref_idx_l0_active-1 :
                               num_ref_idx_l1_active-1);

                if (false) { // do not show these flags
                  for (int i=0;i<=num_ref;i++) {
                    LOG3("luma_weight_flag_l%d[%d]        : %d\n",l,i,luma_weight_flag[l][i]);
                  }

                  if (sps->chroma_format_idc != 0) {
                    for (int i=0;i<=num_ref;i++) {
                      LOG3("chroma_weight_flag_l%d[%d]      : %d\n",l,i,chroma_weight_flag[l][i]);
                    }
                  }
                }

                for (int i=0;i<=num_ref;i++) {
                  LOG3("LumaWeight_L%d[%d]             : %d\n",l,i,LumaWeight[l][i]);
                  LOG3("luma_offset_l%d[%d]            : %d\n",l,i,luma_offset[l][i]);

                  for (int j=0;j<2;j++) {
                    LOG4("ChromaWeight_L%d[%d][%d]        : %d\n",l,i,j,ChromaWeight[l][i][j]);
                    LOG4("ChromaOffset_L%d[%d][%d]        : %d\n",l,i,j,ChromaOffset[l][i][j]);
                  }
                }
              }
        }

      LOG1("five_minus_max_num_merge_cand  : %d\n", five_minus_max_num_merge_cand);
    }


    LOG1("slice_qp_delta         : %d\n", slice_qp_delta);
    if (pps->pps_slice_chroma_qp_offsets_present_flag) {
      LOG1("slice_cb_qp_offset     : %d\n", slice_cb_qp_offset);
      LOG1("slice_cr_qp_offset     : %d\n", slice_cr_qp_offset);
    }

    if (pps->deblocking_filter_override_enabled_flag) {
      LOG1("deblocking_filter_override_flag : %d\n", deblocking_filter_override_flag);
    }

    LOG2("slice_deblocking_filter_disabled_flag : %d %s\n",
         slice_deblocking_filter_disabled_flag,
         (deblocking_filter_override_flag ? "(override)" : "(from pps)"));

    if (deblocking_filter_override_flag) {

      if (!slice_deblocking_filter_disabled_flag) {
        LOG1("slice_beta_offset  : %d\n", slice_beta_offset);
        LOG1("slice_tc_offset    : %d\n", slice_tc_offset);
      }
    }

    if (pps->pps_loop_filter_across_slices_enabled_flag  &&
        (slice_sao_luma_flag || slice_sao_chroma_flag ||
         !slice_deblocking_filter_disabled_flag)) {
      LOG1("slice_loop_filter_across_slices_enabled_flag : %d\n",
           slice_loop_filter_across_slices_enabled_flag);
    }
  }

  if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag) {
    LOG1("num_entry_point_offsets    : %d\n", num_entry_point_offsets);

    if (num_entry_point_offsets > 0) {
      LOG1("offset_len                 : %d\n", offset_len);

      for (int i=0; i<num_entry_point_offsets; i++) {
        LOG2("entry point [%i] : %d\n", i, entry_point_offset[i]);
      }
    }
  }

  /*
    if( slice_segment_header_extension_present_flag ) {
    slice_segment_header_extension_length
    for( i = 0; i < slice_segment_header_extension_length; i++)
    slice_segment_header_extension_data_byte[i]
    }
    byte_alignment()
    }
  */

#undef LOG0
#undef LOG1
#undef LOG2
#undef LOG3
#undef LOG4
  //#endif
}