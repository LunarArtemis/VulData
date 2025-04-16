int ff_h264_decode_seq_parameter_set(H264Context *h){
    MpegEncContext * const s = &h->s;
    int profile_idc, level_idc, constraint_set_flags = 0;
    unsigned int sps_id;
    int i;
    SPS *sps;

    profile_idc= get_bits(&s->gb, 8);
    constraint_set_flags |= get_bits1(&s->gb) << 0;   //constraint_set0_flag
    constraint_set_flags |= get_bits1(&s->gb) << 1;   //constraint_set1_flag
    constraint_set_flags |= get_bits1(&s->gb) << 2;   //constraint_set2_flag
    constraint_set_flags |= get_bits1(&s->gb) << 3;   //constraint_set3_flag
    get_bits(&s->gb, 4); // reserved
    level_idc= get_bits(&s->gb, 8);
    sps_id= get_ue_golomb_31(&s->gb);

    if(sps_id >= MAX_SPS_COUNT) {
        av_log(h->s.avctx, AV_LOG_ERROR, "sps_id (%d) out of range\n", sps_id);
        return -1;
    }
    sps= av_mallocz(sizeof(SPS));
    if(sps == NULL)
        return -1;

    sps->time_offset_length = 24;
    sps->profile_idc= profile_idc;
    sps->constraint_set_flags = constraint_set_flags;
    sps->level_idc= level_idc;
    sps->full_range = -1;

    memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
    memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
    sps->scaling_matrix_present = 0;
    sps->colorspace = 2; //AVCOL_SPC_UNSPECIFIED

    if(sps->profile_idc >= 100){ //high profile
        sps->chroma_format_idc= get_ue_golomb_31(&s->gb);
        if(sps->chroma_format_idc == 3)
            sps->residual_color_transform_flag = get_bits1(&s->gb);
        sps->bit_depth_luma   = get_ue_golomb(&s->gb) + 8;
        sps->bit_depth_chroma = get_ue_golomb(&s->gb) + 8;
        if (sps->bit_depth_luma > 12U || sps->bit_depth_chroma > 12U) {
            av_log(h->s.avctx, AV_LOG_ERROR, "illegal bit depth value (%d, %d)\n",
                   sps->bit_depth_luma, sps->bit_depth_chroma);
            goto fail;
        }
        sps->transform_bypass = get_bits1(&s->gb);
        decode_scaling_matrices(h, sps, NULL, 1, sps->scaling_matrix4, sps->scaling_matrix8);
    }else{
        sps->chroma_format_idc= 1;
        sps->bit_depth_luma   = 8;
        sps->bit_depth_chroma = 8;
    }

    sps->log2_max_frame_num= get_ue_golomb(&s->gb) + 4;
    sps->poc_type= get_ue_golomb_31(&s->gb);

    if(sps->poc_type == 0){ //FIXME #define
        sps->log2_max_poc_lsb= get_ue_golomb(&s->gb) + 4;
    } else if(sps->poc_type == 1){//FIXME #define
        sps->delta_pic_order_always_zero_flag= get_bits1(&s->gb);
        sps->offset_for_non_ref_pic= get_se_golomb(&s->gb);
        sps->offset_for_top_to_bottom_field= get_se_golomb(&s->gb);
        sps->poc_cycle_length                = get_ue_golomb(&s->gb);

        if((unsigned)sps->poc_cycle_length >= FF_ARRAY_ELEMS(sps->offset_for_ref_frame)){
            av_log(h->s.avctx, AV_LOG_ERROR, "poc_cycle_length overflow %u\n", sps->poc_cycle_length);
            goto fail;
        }

        for(i=0; i<sps->poc_cycle_length; i++)
            sps->offset_for_ref_frame[i]= get_se_golomb(&s->gb);
    }else if(sps->poc_type != 2){
        av_log(h->s.avctx, AV_LOG_ERROR, "illegal POC type %d\n", sps->poc_type);
        goto fail;
    }

    sps->ref_frame_count= get_ue_golomb_31(&s->gb);
    if(sps->ref_frame_count > MAX_PICTURE_COUNT-2 || sps->ref_frame_count > 16U){
        av_log(h->s.avctx, AV_LOG_ERROR, "too many reference frames\n");
        goto fail;
    }
    sps->gaps_in_frame_num_allowed_flag= get_bits1(&s->gb);
    sps->mb_width = get_ue_golomb(&s->gb) + 1;
    sps->mb_height= get_ue_golomb(&s->gb) + 1;
    if((unsigned)sps->mb_width >= INT_MAX/16 || (unsigned)sps->mb_height >= INT_MAX/16 ||
       av_image_check_size(16*sps->mb_width, 16*sps->mb_height, 0, h->s.avctx)){
        av_log(h->s.avctx, AV_LOG_ERROR, "mb_width/height overflow\n");
        goto fail;
    }

    sps->frame_mbs_only_flag= get_bits1(&s->gb);
    if(!sps->frame_mbs_only_flag)
        sps->mb_aff= get_bits1(&s->gb);
    else
        sps->mb_aff= 0;

    sps->direct_8x8_inference_flag= get_bits1(&s->gb);

#ifndef ALLOW_INTERLACE
    if(sps->mb_aff)
        av_log(h->s.avctx, AV_LOG_ERROR, "MBAFF support not included; enable it at compile-time.\n");
#endif
    sps->crop= get_bits1(&s->gb);
    if(sps->crop){
        int crop_vertical_limit   = sps->chroma_format_idc  & 2 ? 16 : 8;
        int crop_horizontal_limit = sps->chroma_format_idc == 3 ? 16 : 8;
        sps->crop_left  = get_ue_golomb(&s->gb);
        sps->crop_right = get_ue_golomb(&s->gb);
        sps->crop_top   = get_ue_golomb(&s->gb);
        sps->crop_bottom= get_ue_golomb(&s->gb);
        if(sps->crop_left || sps->crop_top){
            av_log(h->s.avctx, AV_LOG_ERROR, "insane cropping not completely supported, this could look slightly wrong ... (left: %d, top: %d)\n", sps->crop_left, sps->crop_top);
        }
        if(sps->crop_right >= crop_horizontal_limit || sps->crop_bottom >= crop_vertical_limit){
            av_log(h->s.avctx, AV_LOG_ERROR, "brainfart cropping not supported, cropping disabled (right: %d, bottom: %d)\n", sps->crop_right, sps->crop_bottom);
        /* It is very unlikely that partial cropping will make anybody happy.
         * Not cropping at all fixes for example playback of Sisvel 3D streams
         * in applications supporting Sisvel 3D. */
        sps->crop_left  =
        sps->crop_right =
        sps->crop_top   =
        sps->crop_bottom= 0;
        }
    }else{
        sps->crop_left  =
        sps->crop_right =
        sps->crop_top   =
        sps->crop_bottom= 0;
    }

    sps->vui_parameters_present_flag= get_bits1(&s->gb);
    if( sps->vui_parameters_present_flag )
        if (decode_vui_parameters(h, sps) < 0)
            goto fail;

    if(!sps->sar.den)
        sps->sar.den= 1;

    if(s->avctx->debug&FF_DEBUG_PICT_INFO){
        av_log(h->s.avctx, AV_LOG_DEBUG, "sps:%u profile:%d/%d poc:%d ref:%d %dx%d %s %s crop:%d/%d/%d/%d %s %s %d/%d b%d\n",
               sps_id, sps->profile_idc, sps->level_idc,
               sps->poc_type,
               sps->ref_frame_count,
               sps->mb_width, sps->mb_height,
               sps->frame_mbs_only_flag ? "FRM" : (sps->mb_aff ? "MB-AFF" : "PIC-AFF"),
               sps->direct_8x8_inference_flag ? "8B8" : "",
               sps->crop_left, sps->crop_right,
               sps->crop_top, sps->crop_bottom,
               sps->vui_parameters_present_flag ? "VUI" : "",
               ((const char*[]){"Gray","420","422","444"})[sps->chroma_format_idc],
               sps->timing_info_present_flag ? sps->num_units_in_tick : 0,
               sps->timing_info_present_flag ? sps->time_scale : 0,
               sps->bit_depth_luma
               );
    }

    av_free(h->sps_buffers[sps_id]);
    h->sps_buffers[sps_id]= sps;
    h->sps = *sps;
    return 0;
fail:
    av_free(sps);
    return -1;
}