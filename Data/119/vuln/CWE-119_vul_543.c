static OPJ_BOOL opj_j2k_read_SPCod_SPCoc(  opj_j2k_t *p_j2k,
                                                                OPJ_UINT32 compno,
                                                                OPJ_BYTE * p_header_data,
                                                                OPJ_UINT32 * p_header_size,
                                                                opj_event_mgr_t * p_manager)
{
        OPJ_UINT32 i, l_tmp;
        opj_cp_t *l_cp = NULL;
        opj_tcp_t *l_tcp = NULL;
        opj_tccp_t *l_tccp = NULL;
        OPJ_BYTE * l_current_ptr = NULL;

        /* preconditions */
        assert(p_j2k != 00);
        assert(p_manager != 00);
        assert(p_header_data != 00);

        l_cp = &(p_j2k->m_cp);
        l_tcp = (p_j2k->m_specific_param.m_decoder.m_state == J2K_STATE_TPH) ?
                                &l_cp->tcps[p_j2k->m_current_tile_number] :
                                p_j2k->m_specific_param.m_decoder.m_default_tcp;

        /* precondition again */
        assert(compno < p_j2k->m_private_image->numcomps);

        l_tccp = &l_tcp->tccps[compno];
        l_current_ptr = p_header_data;

        /* make sure room is sufficient */
        if (*p_header_size < 5) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading SPCod SPCoc element\n");
                return OPJ_FALSE;
        }

        opj_read_bytes(l_current_ptr, &l_tccp->numresolutions ,1);              /* SPcox (D) */
        ++l_tccp->numresolutions;                                                                               /* tccp->numresolutions = read() + 1 */
        if (l_tccp->numresolutions > OPJ_J2K_MAXRLVLS) {
                opj_event_msg(p_manager, EVT_ERROR,
                              "Invalid value for numresolutions : %d, max value is set in openjpeg.h at %d\n",
                              l_tccp->numresolutions, OPJ_J2K_MAXRLVLS);
                return OPJ_FALSE;
        }
        ++l_current_ptr;

        /* If user wants to remove more resolutions than the codestream contains, return error */
        if (l_cp->m_specific_param.m_dec.m_reduce >= l_tccp->numresolutions) {
                opj_event_msg(p_manager, EVT_ERROR, "Error decoding component %d.\nThe number of resolutions to remove is higher than the number "
                                        "of resolutions of this component\nModify the cp_reduce parameter.\n\n", compno);
                p_j2k->m_specific_param.m_decoder.m_state |= 0x8000;/* FIXME J2K_DEC_STATE_ERR;*/
                return OPJ_FALSE;
        }

        opj_read_bytes(l_current_ptr,&l_tccp->cblkw ,1);                /* SPcoc (E) */
        ++l_current_ptr;
        l_tccp->cblkw += 2;

        opj_read_bytes(l_current_ptr,&l_tccp->cblkh ,1);                /* SPcoc (F) */
        ++l_current_ptr;
        l_tccp->cblkh += 2;

        if ((l_tccp->cblkw > 10) || (l_tccp->cblkh > 10) || ((l_tccp->cblkw + l_tccp->cblkh) > 12)) {
                opj_event_msg(p_manager, EVT_ERROR, "Error reading SPCod SPCoc element, Invalid cblkw/cblkh combination\n");
                return OPJ_FALSE;
        }
	

        opj_read_bytes(l_current_ptr,&l_tccp->cblksty ,1);              /* SPcoc (G) */
        ++l_current_ptr;

        opj_read_bytes(l_current_ptr,&l_tccp->qmfbid ,1);               /* SPcoc (H) */
        ++l_current_ptr;

        *p_header_size = *p_header_size - 5;

        /* use custom precinct size ? */
        if (l_tccp->csty & J2K_CCP_CSTY_PRT) {
                if (*p_header_size < l_tccp->numresolutions) {
                        opj_event_msg(p_manager, EVT_ERROR, "Error reading SPCod SPCoc element\n");
                        return OPJ_FALSE;
                }

                for     (i = 0; i < l_tccp->numresolutions; ++i) {
                        opj_read_bytes(l_current_ptr,&l_tmp ,1);                /* SPcoc (I_i) */
                        ++l_current_ptr;
                        /* Precinct exponent 0 is only allowed for lowest resolution level (Table A.21) */
                        if ((i != 0) && (((l_tmp & 0xf) == 0) || ((l_tmp >> 4) == 0))) {
                                opj_event_msg(p_manager, EVT_ERROR, "Invalid precinct size\n");
                                return OPJ_FALSE;
                        }
                        l_tccp->prcw[i] = l_tmp & 0xf;
                        l_tccp->prch[i] = l_tmp >> 4;
                }

                *p_header_size = *p_header_size - l_tccp->numresolutions;
        }
        else {
                /* set default size for the precinct width and height */
                for     (i = 0; i < l_tccp->numresolutions; ++i) {
                        l_tccp->prcw[i] = 15;
                        l_tccp->prch[i] = 15;
                }
        }

#ifdef WIP_REMOVE_MSD
        /* INDEX >> */
        if (p_j2k->cstr_info && compno == 0) {
                OPJ_UINT32 l_data_size = l_tccp->numresolutions * sizeof(OPJ_UINT32);

                p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].tccp_info[compno].cblkh = l_tccp->cblkh;
                p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].tccp_info[compno].cblkw = l_tccp->cblkw;
                p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].tccp_info[compno].numresolutions = l_tccp->numresolutions;
                p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].tccp_info[compno].cblksty = l_tccp->cblksty;
                p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].tccp_info[compno].qmfbid = l_tccp->qmfbid;

                memcpy(p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].pdx,l_tccp->prcw, l_data_size);
                memcpy(p_j2k->cstr_info->tile[p_j2k->m_current_tile_number].pdy,l_tccp->prch, l_data_size);
        }
        /* << INDEX */
#endif

        return OPJ_TRUE;
}