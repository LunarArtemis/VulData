static OPJ_BOOL opj_j2k_read_SQcd_SQcc(opj_j2k_t *p_j2k,
                                                            OPJ_UINT32 p_comp_no,
                                                            OPJ_BYTE* p_header_data,
                                                            OPJ_UINT32 * p_header_size,
                                                            opj_event_mgr_t * p_manager
                                                            )
{
        /* loop*/
        OPJ_UINT32 l_band_no;
        opj_cp_t *l_cp = 00;
        opj_tcp_t *l_tcp = 00;
        opj_tccp_t *l_tccp = 00;
        OPJ_BYTE * l_current_ptr = 00;
        OPJ_UINT32 l_tmp, l_num_band;

        /* preconditions*/
        assert(p_j2k != 00);
        assert(p_manager != 00);
        assert(p_header_data != 00);

        l_cp = &(p_j2k->m_cp);
        /* come from tile part header or main header ?*/
        l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH) ? /*FIXME J2K_DEC_STATE_TPH*/
                                &l_cp->tcps[p_j2k->m_current_tile_number] :
                                p_j2k->m_specific_param.m_decoder.m_default_tcp;

        /* precondition again*/
        assert(p_comp_no <  p_j2k->m_private_image->numcomps);

        l_tccp = &l_tcp->tccps[p_comp_no];
        l_current_ptr = p_header_data;

        if (*p_header_size < 1) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading SQcd or SQcc element\n");
                return OPJ_FALSE;
        }
        *p_header_size -= 1;

        opj_read_bytes(l_current_ptr, &l_tmp ,1);                       /* Sqcx */
        ++l_current_ptr;

        l_tccp->qntsty = l_tmp & 0x1f;
        l_tccp->numgbits = l_tmp >> 5;
        if (l_tccp->qntsty == J2K_CCP_QNTSTY_SIQNT) {
        l_num_band = 1;
        }
        else {
                l_num_band = (l_tccp->qntsty == J2K_CCP_QNTSTY_NOQNT) ?
                        (*p_header_size) :
                        (*p_header_size) / 2;

                if( l_num_band > OPJ_J2K_MAXBANDS ) {
                        opj_event_msg(p_manager, EVT_WARNING, "While reading CCP_QNTSTY element inside QCD or QCC marker segment, "
                                "number of subbands (%d) is greater to OPJ_J2K_MAXBANDS (%d). So we limit the number of elements stored to "
                                "OPJ_J2K_MAXBANDS (%d) and skip the rest. \n", l_num_band, OPJ_J2K_MAXBANDS, OPJ_J2K_MAXBANDS);
                        /*return OPJ_FALSE;*/
                }
        }

#ifdef USE_JPWL
        if (l_cp->correct) {

                /* if JPWL is on, we check whether there are too many subbands */
                if (/*(l_num_band < 0) ||*/ (l_num_band >= OPJ_J2K_MAXBANDS)) {
                        opj_event_msg(p_manager, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR,
                                "JPWL: bad number of subbands in Sqcx (%d)\n",
                                l_num_band);
                        if (!JPWL_ASSUME) {
                                opj_event_msg(p_manager, EVT_ERROR, "JPWL: giving up\n");
                                return OPJ_FALSE;
                        }
                        /* we try to correct */
                        l_num_band = 1;
                        opj_event_msg(p_manager, EVT_WARNING, "- trying to adjust them\n"
                                "- setting number of bands to %d => HYPOTHESIS!!!\n",
                                l_num_band);
                };

        };
#endif /* USE_JPWL */

        if (l_tccp->qntsty == J2K_CCP_QNTSTY_NOQNT) {
                for     (l_band_no = 0; l_band_no < l_num_band; l_band_no++) {
                        opj_read_bytes(l_current_ptr, &l_tmp ,1);                       /* SPqcx_i */
                        ++l_current_ptr;
                        if (l_band_no < OPJ_J2K_MAXBANDS){
                                l_tccp->stepsizes[l_band_no].expn = (OPJ_INT32)(l_tmp >> 3);
                                l_tccp->stepsizes[l_band_no].mant = 0;
                        }
                }
                *p_header_size = *p_header_size - l_num_band;
        }
        else {
                for     (l_band_no = 0; l_band_no < l_num_band; l_band_no++) {
                        opj_read_bytes(l_current_ptr, &l_tmp ,2);                       /* SPqcx_i */
                        l_current_ptr+=2;
                        if (l_band_no < OPJ_J2K_MAXBANDS){
                                l_tccp->stepsizes[l_band_no].expn = (OPJ_INT32)(l_tmp >> 11);
                                l_tccp->stepsizes[l_band_no].mant = l_tmp & 0x7ff;
                        }
                }
                *p_header_size = *p_header_size - 2*l_num_band;
        }

        /* Add Antonin : if scalar_derived -> compute other stepsizes */
        if (l_tccp->qntsty == J2K_CCP_QNTSTY_SIQNT) {
                for (l_band_no = 1; l_band_no < OPJ_J2K_MAXBANDS; l_band_no++) {
                        l_tccp->stepsizes[l_band_no].expn =
                                ((OPJ_INT32)(l_tccp->stepsizes[0].expn) - (OPJ_INT32)((l_band_no - 1) / 3) > 0) ?
                                        (OPJ_INT32)(l_tccp->stepsizes[0].expn) - (OPJ_INT32)((l_band_no - 1) / 3) : 0;
                        l_tccp->stepsizes[l_band_no].mant = l_tccp->stepsizes[0].mant;
                }
        }

        return OPJ_TRUE;
}