WORD32 ih264d_parse_pslice(dec_struct_t *ps_dec, UWORD16 u2_first_mb_in_slice)
{
    dec_pic_params_t * ps_pps = ps_dec->ps_cur_pps;
    dec_slice_params_t * ps_cur_slice = ps_dec->ps_cur_slice;
    dec_bit_stream_t *ps_bitstrm = ps_dec->ps_bitstrm;
    UWORD32 *pu4_bitstrm_buf = ps_bitstrm->pu4_buffer;
    UWORD32 *pu4_bitstrm_ofst = &ps_bitstrm->u4_ofst;
    UWORD8 u1_mbaff = ps_dec->ps_cur_slice->u1_mbaff_frame_flag; //ps_dec->ps_cur_sps->u1_mb_aff_flag;
    UWORD8 u1_field_pic_flag = ps_cur_slice->u1_field_pic_flag;

    UWORD32 u4_temp;
    WORD32 i_temp;
    WORD32 ret;

    /*--------------------------------------------------------------------*/
    /* Read remaining contents of the slice header                        */
    /*--------------------------------------------------------------------*/
    {
        WORD8 *pi1_buf;
        WORD16 *pi2_mv = ps_dec->s_default_mv_pred.i2_mv;
        WORD32 *pi4_mv = (WORD32*)pi2_mv;
        WORD16 *pi16_refFrame;

        pi1_buf = ps_dec->s_default_mv_pred.i1_ref_frame;
        pi16_refFrame = (WORD16*)pi1_buf;
        *pi4_mv = 0;
        *(pi4_mv + 1) = 0;
        *pi16_refFrame = OUT_OF_RANGE_REF;
        ps_dec->s_default_mv_pred.u1_col_ref_pic_idx = (UWORD8)-1;
        ps_dec->s_default_mv_pred.u1_pic_type = (UWORD8)-1;
    }

    ps_cur_slice->u1_num_ref_idx_active_override_flag = ih264d_get_bit_h264(
                    ps_bitstrm);

    COPYTHECONTEXT("SH: num_ref_idx_override_flag",
                    ps_cur_slice->u1_num_ref_idx_active_override_flag);

    u4_temp = ps_dec->ps_cur_pps->u1_num_ref_idx_lx_active[0];
    if(ps_cur_slice->u1_num_ref_idx_active_override_flag)
    {
        u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf) + 1;
    }

    {



        UWORD8 u1_max_ref_idx = MAX_FRAMES << u1_field_pic_flag;
        if(u4_temp > u1_max_ref_idx)
        {
            return ERROR_NUM_REF;
        }
        ps_cur_slice->u1_num_ref_idx_lx_active[0] = u4_temp;
        COPYTHECONTEXT("SH: num_ref_idx_l0_active_minus1",
                        ps_cur_slice->u1_num_ref_idx_lx_active[0] - 1);

    }

    {
        UWORD8 uc_refIdxReFlagL0 = ih264d_get_bit_h264(ps_bitstrm);
        COPYTHECONTEXT("SH: ref_pic_list_reordering_flag_l0",uc_refIdxReFlagL0);

        /* Initialize the Reference list once in Picture if the slice type    */
        /* of first slice is between 5 to 9 defined in table 7.3 of standard  */
        /* If picture contains both P & B slices then Initialize the Reference*/
        /* List only when it switches from P to B and B to P                     */
        {
            UWORD8 init_idx_flg = (ps_dec->u1_pr_sl_type
                            != ps_dec->ps_cur_slice->u1_slice_type);
            if(ps_dec->u1_first_pb_nal_in_pic
                            || (init_idx_flg & !ps_dec->u1_sl_typ_5_9)
                            || ps_dec->u1_num_ref_idx_lx_active_prev
                                            != ps_cur_slice->u1_num_ref_idx_lx_active[0])
            {
                ih264d_init_ref_idx_lx_p(ps_dec);
            }
            if(ps_dec->u1_first_pb_nal_in_pic & ps_dec->u1_sl_typ_5_9)
                ps_dec->u1_first_pb_nal_in_pic = 0;
        }
        /* Store the value for future slices in the same picture */
        ps_dec->u1_num_ref_idx_lx_active_prev =
                        ps_cur_slice->u1_num_ref_idx_lx_active[0];

        /* Modified temporarily */
        if(uc_refIdxReFlagL0)
        {
            WORD8 ret;
            ps_dec->ps_ref_pic_buf_lx[0] = ps_dec->ps_dpb_mgr->ps_mod_dpb[0];
            ret = ih264d_ref_idx_reordering(ps_dec, 0);
            if(ret == -1)
                return ERROR_REFIDX_ORDER_T;
            ps_dec->ps_ref_pic_buf_lx[0] = ps_dec->ps_dpb_mgr->ps_mod_dpb[0];
        }
        else
            ps_dec->ps_ref_pic_buf_lx[0] =
                            ps_dec->ps_dpb_mgr->ps_init_dpb[0];
    }
    /* Create refIdx to POC mapping */
    {
        void **pui_map_ref_idx_to_poc_lx0, **pui_map_ref_idx_to_poc_lx1;
        WORD8 idx;
        struct pic_buffer_t *ps_pic;

        pui_map_ref_idx_to_poc_lx0 = ps_dec->ppv_map_ref_idx_to_poc + FRM_LIST_L0;
        pui_map_ref_idx_to_poc_lx0[0] = 0; //For ref_idx = -1
        pui_map_ref_idx_to_poc_lx0++;
        for(idx = 0; idx < ps_cur_slice->u1_num_ref_idx_lx_active[0]; idx++)
        {
            ps_pic = ps_dec->ps_ref_pic_buf_lx[0][idx];
            pui_map_ref_idx_to_poc_lx0[idx] = (ps_pic->pu1_buf1);
        }

        /* Bug Fix Deblocking */
        pui_map_ref_idx_to_poc_lx1 = ps_dec->ppv_map_ref_idx_to_poc + FRM_LIST_L1;
        pui_map_ref_idx_to_poc_lx1[0] = 0;

        if(u1_mbaff)
        {
            void **ppv_map_ref_idx_to_poc_lx_t, **ppv_map_ref_idx_to_poc_lx_b;
            void **ppv_map_ref_idx_to_poc_lx_t1, **ppv_map_ref_idx_to_poc_lx_b1;
            ppv_map_ref_idx_to_poc_lx_t = ps_dec->ppv_map_ref_idx_to_poc
                            + TOP_LIST_FLD_L0;
            ppv_map_ref_idx_to_poc_lx_b = ps_dec->ppv_map_ref_idx_to_poc
                            + BOT_LIST_FLD_L0;

            ppv_map_ref_idx_to_poc_lx_t[0] = 0; //  For ref_idx = -1
            ppv_map_ref_idx_to_poc_lx_t++;
            ppv_map_ref_idx_to_poc_lx_b[0] = 0; // For ref_idx = -1
            ppv_map_ref_idx_to_poc_lx_b++;

            idx = 0;
            for(idx = 0; idx < ps_cur_slice->u1_num_ref_idx_lx_active[0]; idx++)
            {
                ps_pic = ps_dec->ps_ref_pic_buf_lx[0][idx];
                ppv_map_ref_idx_to_poc_lx_t[0] = (ps_pic->pu1_buf1);
                ppv_map_ref_idx_to_poc_lx_b[1] = (ps_pic->pu1_buf1);

                ppv_map_ref_idx_to_poc_lx_b[0] = (ps_pic->pu1_buf1) + 1;
                ppv_map_ref_idx_to_poc_lx_t[1] = (ps_pic->pu1_buf1) + 1;

                ppv_map_ref_idx_to_poc_lx_t += 2;
                ppv_map_ref_idx_to_poc_lx_b += 2;
            }
            ppv_map_ref_idx_to_poc_lx_t1 = ps_dec->ppv_map_ref_idx_to_poc
                            + TOP_LIST_FLD_L1;
            ppv_map_ref_idx_to_poc_lx_t1[0] = 0;
            ppv_map_ref_idx_to_poc_lx_b1 = ps_dec->ppv_map_ref_idx_to_poc
                            + BOT_LIST_FLD_L1;
            ppv_map_ref_idx_to_poc_lx_b1[0] = 0;

        }

        if(ps_dec->u4_num_cores >= 3)
        {
            WORD32 num_entries;
            WORD32 size;

            num_entries = MIN(MAX_FRAMES, ps_dec->u4_num_ref_frames_at_init);
            num_entries = 2 * ((2 * num_entries) + 1);

            size = num_entries * sizeof(void *);
            size += PAD_MAP_IDX_POC * sizeof(void *);

            memcpy((void *)ps_dec->ps_parse_cur_slice->ppv_map_ref_idx_to_poc,
                   ps_dec->ppv_map_ref_idx_to_poc,
                   size);
        }


    }
    if(ps_pps->u1_wted_pred_flag)
    {
        ret = ih264d_parse_pred_weight_table(ps_cur_slice, ps_bitstrm);
        if(ret != OK)
            return ret;
        ih264d_form_pred_weight_matrix(ps_dec);
        ps_dec->pu4_wt_ofsts = ps_dec->pu4_wts_ofsts_mat;
    }
    else
    {
        ps_dec->ps_cur_slice->u2_log2Y_crwd = 0;
        ps_dec->pu4_wt_ofsts = ps_dec->pu4_wts_ofsts_mat;
    }

    ps_dec->ps_parse_cur_slice->u2_log2Y_crwd =
                    ps_dec->ps_cur_slice->u2_log2Y_crwd;

    if(u1_mbaff && (u1_field_pic_flag == 0))
    {
        ih264d_convert_frm_mbaff_list(ps_dec);
    }

    /* G050 */
    if(ps_cur_slice->u1_nal_ref_idc != 0)
    {
        if(!ps_dec->ps_dpb_cmds->u1_dpb_commands_read)
            ps_dec->u4_bitoffset = ih264d_read_mmco_commands(ps_dec);
        else
            ps_bitstrm->u4_ofst += ps_dec->u4_bitoffset;

    }
    /* G050 */

    if(ps_pps->u1_entropy_coding_mode == CABAC)
    {
        u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);

        if(u4_temp > MAX_CABAC_INIT_IDC)
        {
            return ERROR_INV_SLICE_HDR_T;
        }
        ps_cur_slice->u1_cabac_init_idc = u4_temp;
        COPYTHECONTEXT("SH: cabac_init_idc",ps_cur_slice->u1_cabac_init_idc);
    }

    /* Read slice_qp_delta */
    i_temp = ps_pps->u1_pic_init_qp
                    + ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
    if((i_temp < 0) || (i_temp > 51))
    {
        return ERROR_INV_RANGE_QP_T;
    }
    ps_cur_slice->u1_slice_qp = i_temp;
    COPYTHECONTEXT("SH: slice_qp_delta",
                    (WORD8)(ps_cur_slice->u1_slice_qp - ps_pps->u1_pic_init_qp));

    if(ps_pps->u1_deblocking_filter_parameters_present_flag == 1)
    {
        u4_temp = ih264d_uev(pu4_bitstrm_ofst, pu4_bitstrm_buf);
        if(u4_temp > SLICE_BOUNDARY_DBLK_DISABLED)
        {
            return ERROR_INV_SLICE_HDR_T;
        }

        COPYTHECONTEXT("SH: disable_deblocking_filter_idc", u4_temp);
        ps_cur_slice->u1_disable_dblk_filter_idc = u4_temp;
        if(u4_temp != 1)
        {
            i_temp = ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf)
                            << 1;
            if((MIN_DBLK_FIL_OFF > i_temp) || (i_temp > MAX_DBLK_FIL_OFF))
            {
                return ERROR_INV_SLICE_HDR_T;
            }
            ps_cur_slice->i1_slice_alpha_c0_offset = i_temp;
            COPYTHECONTEXT("SH: slice_alpha_c0_offset_div2",
                            ps_cur_slice->i1_slice_alpha_c0_offset >> 1);

            i_temp = ih264d_sev(pu4_bitstrm_ofst, pu4_bitstrm_buf)
                            << 1;
            if((MIN_DBLK_FIL_OFF > i_temp) || (i_temp > MAX_DBLK_FIL_OFF))
            {
                return ERROR_INV_SLICE_HDR_T;
            }
            ps_cur_slice->i1_slice_beta_offset = i_temp;
            COPYTHECONTEXT("SH: slice_beta_offset_div2",
                            ps_cur_slice->i1_slice_beta_offset >> 1);
        }
        else
        {
            ps_cur_slice->i1_slice_alpha_c0_offset = 0;
            ps_cur_slice->i1_slice_beta_offset = 0;
        }
    }
    else
    {
        ps_cur_slice->u1_disable_dblk_filter_idc = 0;
        ps_cur_slice->i1_slice_alpha_c0_offset = 0;
        ps_cur_slice->i1_slice_beta_offset = 0;
    }

    ps_dec->u1_slice_header_done = 2;

    if(ps_pps->u1_entropy_coding_mode)
    {
        SWITCHOFFTRACE; SWITCHONTRACECABAC;
        ps_dec->pf_parse_inter_slice = ih264d_parse_inter_slice_data_cabac;
        ps_dec->pf_parse_inter_mb = ih264d_parse_pmb_cabac;
        ih264d_init_cabac_contexts(P_SLICE, ps_dec);

        if(ps_dec->ps_cur_slice->u1_mbaff_frame_flag)
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cabac_mbaff;
        else
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cabac_nonmbaff;
    }
    else
    {
        SWITCHONTRACE; SWITCHOFFTRACECABAC;
        ps_dec->pf_parse_inter_slice = ih264d_parse_inter_slice_data_cavlc;
        ps_dec->pf_parse_inter_mb = ih264d_parse_pmb_cavlc;
        if(ps_dec->ps_cur_slice->u1_mbaff_frame_flag)
        {
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cavlc_mbaff;
        }
        else
            ps_dec->pf_get_mb_info = ih264d_get_mb_info_cavlc_nonmbaff;
    }

    ps_dec->u1_B = 0;
    ps_dec->pf_mvpred_ref_tfr_nby2mb = ih264d_mv_pred_ref_tfr_nby2_pmb;
    ret = ps_dec->pf_parse_inter_slice(ps_dec, ps_cur_slice, u2_first_mb_in_slice);
    if(ret != OK)
        return ret;
//    ps_dec->curr_slice_in_error = 0 ;
    return OK;
}