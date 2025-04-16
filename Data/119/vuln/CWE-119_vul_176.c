uint8_t reconstruct_single_channel(NeAACDecStruct *hDecoder, ic_stream *ics,
                                   element *sce, int16_t *spec_data)
{
    uint8_t retval;
    int output_channels;
    ALIGN real_t spec_coef[1024];

#ifdef PROFILE
    int64_t count = faad_get_ts();
#endif


    /* always allocate 2 channels, PS can always "suddenly" turn up */
#if ( (defined(DRM) && defined(DRM_PS)) )
    output_channels = 2;
#elif defined(PS_DEC)
    if (hDecoder->ps_used[hDecoder->fr_ch_ele])
        output_channels = 2;
    else
        output_channels = 1;
#else
    output_channels = 1;
#endif

    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 0)
    {
        /* element_output_channels not set yet */
        hDecoder->element_output_channels[hDecoder->fr_ch_ele] = output_channels;
    } else if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] != output_channels) {
        /* element inconsistency */

        /* this only happens if PS is actually found but not in the first frame
         * this means that there is only 1 bitstream element!
         */

        /* reset the allocation */
        hDecoder->element_alloced[hDecoder->fr_ch_ele] = 0;

        hDecoder->element_output_channels[hDecoder->fr_ch_ele] = output_channels;

        //return 21;
    }

    if (hDecoder->element_alloced[hDecoder->fr_ch_ele] == 0)
    {
        retval = allocate_single_channel(hDecoder, sce->channel, output_channels);
        if (retval > 0)
            return retval;

        hDecoder->element_alloced[hDecoder->fr_ch_ele] = 1;
    }


    /* dequantisation and scaling */
    retval = quant_to_spec(hDecoder, ics, spec_data, spec_coef, hDecoder->frameLength);
    if (retval > 0)
        return retval;

#ifdef PROFILE
    count = faad_get_ts() - count;
    hDecoder->requant_cycles += count;
#endif


    /* pns decoding */
    pns_decode(ics, NULL, spec_coef, NULL, hDecoder->frameLength, 0, hDecoder->object_type,
        &(hDecoder->__r1), &(hDecoder->__r2));

#ifdef MAIN_DEC
    /* MAIN object type prediction */
    if (hDecoder->object_type == MAIN)
    {
		if (!hDecoder->pred_stat[sce->channel])
			return 33;

        /* intra channel prediction */
        ic_prediction(ics, spec_coef, hDecoder->pred_stat[sce->channel], hDecoder->frameLength,
            hDecoder->sf_index);

        /* In addition, for scalefactor bands coded by perceptual
           noise substitution the predictors belonging to the
           corresponding spectral coefficients are reset.
        */
        pns_reset_pred_state(ics, hDecoder->pred_stat[sce->channel]);
    }
#endif

#ifdef LTP_DEC
    if (is_ltp_ot(hDecoder->object_type))
    {
#ifdef LD_DEC
        if (hDecoder->object_type == LD)
        {
            if (ics->ltp.data_present)
            {
                if (ics->ltp.lag_update)
                    hDecoder->ltp_lag[sce->channel] = ics->ltp.lag;
            }
            ics->ltp.lag = hDecoder->ltp_lag[sce->channel];
        }
#endif

        /* long term prediction */
        lt_prediction(ics, &(ics->ltp), spec_coef, hDecoder->lt_pred_stat[sce->channel], hDecoder->fb,
            ics->window_shape, hDecoder->window_shape_prev[sce->channel],
            hDecoder->sf_index, hDecoder->object_type, hDecoder->frameLength);
    }
#endif

    /* tns decoding */
    tns_decode_frame(ics, &(ics->tns), hDecoder->sf_index, hDecoder->object_type,
        spec_coef, hDecoder->frameLength);

    /* drc decoding */
    if (hDecoder->drc->present)
    {
        if (!hDecoder->drc->exclude_mask[sce->channel] || !hDecoder->drc->excluded_chns_present)
            drc_decode(hDecoder->drc, spec_coef);
    }

    /* filter bank */
#ifdef SSR_DEC
    if (hDecoder->object_type != SSR)
    {
#endif
        ifilter_bank(hDecoder->fb, ics->window_sequence, ics->window_shape,
            hDecoder->window_shape_prev[sce->channel], spec_coef,
            hDecoder->time_out[sce->channel], hDecoder->fb_intermed[sce->channel],
            hDecoder->object_type, hDecoder->frameLength);
#ifdef SSR_DEC
    } else {
        ssr_decode(&(ics->ssr), hDecoder->fb, ics->window_sequence, ics->window_shape,
            hDecoder->window_shape_prev[sce->channel], spec_coef, hDecoder->time_out[sce->channel],
            hDecoder->ssr_overlap[sce->channel], hDecoder->ipqf_buffer[sce->channel], hDecoder->prev_fmd[sce->channel],
            hDecoder->frameLength);
    }
#endif

    /* save window shape for next frame */
    hDecoder->window_shape_prev[sce->channel] = ics->window_shape;

#ifdef LTP_DEC
    if (is_ltp_ot(hDecoder->object_type))
    {
        lt_update_state(hDecoder->lt_pred_stat[sce->channel], hDecoder->time_out[sce->channel],
            hDecoder->fb_intermed[sce->channel], hDecoder->frameLength, hDecoder->object_type);
    }
#endif

#ifdef SBR_DEC
    if (((hDecoder->sbr_present_flag == 1) || (hDecoder->forceUpSampling == 1))
        && hDecoder->sbr_alloced[hDecoder->fr_ch_ele])
    {
        int ele = hDecoder->fr_ch_ele;
        int ch = sce->channel;

        /* following case can happen when forceUpSampling == 1 */
        if (hDecoder->sbr[ele] == NULL)
        {
            hDecoder->sbr[ele] = sbrDecodeInit(hDecoder->frameLength,
                hDecoder->element_id[ele], 2*get_sample_rate(hDecoder->sf_index),
                hDecoder->downSampledSBR
#ifdef DRM
                , 0
#endif
                );
        }

        if (sce->ics1.window_sequence == EIGHT_SHORT_SEQUENCE)
            hDecoder->sbr[ele]->maxAACLine = 8*min(sce->ics1.swb_offset[max(sce->ics1.max_sfb-1, 0)], sce->ics1.swb_offset_max);
        else
            hDecoder->sbr[ele]->maxAACLine = min(sce->ics1.swb_offset[max(sce->ics1.max_sfb-1, 0)], sce->ics1.swb_offset_max);

        /* check if any of the PS tools is used */
#if (defined(PS_DEC) || defined(DRM_PS))
        if (hDecoder->ps_used[ele] == 0)
        {
#endif
            retval = sbrDecodeSingleFrame(hDecoder->sbr[ele], hDecoder->time_out[ch],
                hDecoder->postSeekResetFlag, hDecoder->downSampledSBR);
#if (defined(PS_DEC) || defined(DRM_PS))
        } else {
            retval = sbrDecodeSingleFramePS(hDecoder->sbr[ele], hDecoder->time_out[ch],
                hDecoder->time_out[ch+1], hDecoder->postSeekResetFlag,
                hDecoder->downSampledSBR);
        }
#endif
        if (retval > 0)
            return retval;
    } else if (((hDecoder->sbr_present_flag == 1) || (hDecoder->forceUpSampling == 1))
        && !hDecoder->sbr_alloced[hDecoder->fr_ch_ele])
    {
        return 23;
    }
#endif

    /* copy L to R when no PS is used */
#if (defined(PS_DEC) || defined(DRM_PS))
    if ((hDecoder->ps_used[hDecoder->fr_ch_ele] == 0) &&
        (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 2))
    {
        int ele = hDecoder->fr_ch_ele;
        int ch = sce->channel;
        int frame_size = (hDecoder->sbr_alloced[ele]) ? 2 : 1;
        frame_size *= hDecoder->frameLength*sizeof(real_t);

        memcpy(hDecoder->time_out[ch+1], hDecoder->time_out[ch], frame_size);
    }
#endif

    return 0;
}