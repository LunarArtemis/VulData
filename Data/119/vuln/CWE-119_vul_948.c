static void attach_fp_info(packet_info *pinfo, gboolean received, const char *protocol_name, int variant)
{
    int  i=0;
    int  chan;
    int  tf_start, num_chans_start;
    gint node_type;
    int  calculated_variant;

    /* Only need to set info once per session. */
    struct fp_info *p_fp_info = (struct fp_info *)p_get_proto_data(wmem_file_scope(), pinfo, proto_fp, 0);
    if (p_fp_info != NULL) {
        return;
    }

    /* Allocate struct */
    p_fp_info = wmem_new0(wmem_file_scope(), struct fp_info);

    /* Check that the number of outhdr values looks sensible */
    if (((strcmp(protocol_name, "fpiur_r5") == 0) && (outhdr_values_found != 2)) ||
        (outhdr_values_found < 5)) {

        return;
    }

    /* 3gpp release (99, 4, 5, 6, 7) */
    if (strcmp(protocol_name, "fp") == 0) {
        p_fp_info->release = 99;
    }
    else if (strcmp(protocol_name, "fp_r4") == 0) {
        p_fp_info->release = 4;
    }
    else if (strcmp(protocol_name, "fp_r5") == 0) {
        p_fp_info->release = 5;
    }
    else if (strcmp(protocol_name, "fp_r6") == 0) {
        p_fp_info->release = 6;
    }
    else if (strcmp(protocol_name, "fp_r7") == 0) {
        p_fp_info->release = 7;
    }
    else if (strcmp(protocol_name, "fp_r8") == 0) {
        p_fp_info->release = 8;
    }
    else if (strcmp(protocol_name, "fpiur_r5") == 0) {
        p_fp_info->release = 5;
    }
    else {
        /* Really shouldn't get here */
        DISSECTOR_ASSERT_NOT_REACHED();
        return;
    }

    /* Release date is derived from variant number */
    /* Only R6 sub-versions currently influence format within a release */
    switch (p_fp_info->release) {
        case 6:
            if (variant < 256) {
                calculated_variant = variant;
            }
            else {
                calculated_variant = variant / 256;
            }

            switch (calculated_variant) {
                case 1:
                    p_fp_info->release_year = 2005;
                    p_fp_info->release_month = 6;
                    break;
                case 2:
                    p_fp_info->release_year = 2005;
                    p_fp_info->release_month = 9;
                    break;
                case 3:
                default:
                    p_fp_info->release_year = 2006;
                    p_fp_info->release_month = 3;
                    break;
            }
            break;
        case 7:
            p_fp_info->release_year = 2008;
            p_fp_info->release_month = 3;
            break;

        case 8:
            p_fp_info->release_year = 2010;
            p_fp_info->release_month = 6;
            break;


        default:
            p_fp_info->release_year = 0;
            p_fp_info->release_month = 0;
    }


    /* Channel type */
    p_fp_info->channel = outhdr_values[i++];
    /* Sad hack until this value is filled in properly */
    if (p_fp_info->channel == 0) {
        p_fp_info->channel = CHANNEL_DCH;
    }

    /* Derive direction from node type/side */
    node_type = outhdr_values[i++];
    p_fp_info->is_uplink = (( received  && (node_type == 2)) ||
                            (!received  && (node_type == 1)));

    /* Division type introduced for R7 */
    if ((p_fp_info->release == 7) ||
        (p_fp_info->release == 8)) {
        p_fp_info->division = (enum division_type)outhdr_values[i++];
    }

    /* HS-DSCH config */
    if (p_fp_info->channel == CHANNEL_HSDSCH) {
        if ((p_fp_info->release == 7) ||
            (p_fp_info->release == 8)) {
            /* Entity (MAC-hs or MAC-ehs) used */
            if (outhdr_values[i++]) {
                p_fp_info->hsdsch_entity = ehs;
            }
        }
        else {
            /* This is the pre-R7 default */
            p_fp_info->hsdsch_entity = hs;
        }
    }


    /* IUR only uses the above... */
    if (strcmp(protocol_name, "fpiur_r5") == 0) {
        /* Store info in packet */
        p_fp_info->iface_type = IuR_Interface;
        p_add_proto_data(wmem_file_scope(), pinfo, proto_fp, 0, p_fp_info);
        return;
    }

    /* DCH CRC present... */
    p_fp_info->dch_crc_present = outhdr_values[i++];

    /* ... but don't trust for edch */
    if (p_fp_info->channel == CHANNEL_EDCH) {
        p_fp_info->dch_crc_present = 2; /* unknown */
    }

    /* How many paging indications (if PCH data) */
    p_fp_info->paging_indications = outhdr_values[i++];

    /* Number of channels (for coordinated channels) */
    p_fp_info->num_chans = outhdr_values[i++];

    /* EDCH-Common is always T2 */
    if (p_fp_info->channel == CHANNEL_EDCH_COMMON) {
        p_fp_info->edch_type = 1;
    }

    if (p_fp_info->channel != CHANNEL_EDCH) {
        /* TF size for each channel */
        tf_start = i;
        for (chan=0; chan < p_fp_info->num_chans; chan++) {
            p_fp_info->chan_tf_size[chan] = outhdr_values[tf_start+chan];
        }

        /* Number of TBs for each channel */
        num_chans_start = tf_start + p_fp_info->num_chans;
        for (chan=0; chan < p_fp_info->num_chans; chan++) {
            p_fp_info->chan_num_tbs[chan] = outhdr_values[num_chans_start+chan];
        }
    }
    /* EDCH info */
    else {
        int n;

        p_fp_info->no_ddi_entries = outhdr_values[i++];

        /* DDI values */
        for (n=0; n < p_fp_info->no_ddi_entries; n++) {
            p_fp_info->edch_ddi[n] = outhdr_values[i++];
        }

        /* Corresponding MAC-d sizes */
        for (n=0; n < p_fp_info->no_ddi_entries; n++) {
            p_fp_info->edch_macd_pdu_size[n] = outhdr_values[i++];
        }

        if (strcmp(protocol_name, "fp_r8") == 0) {
            p_fp_info->edch_type = outhdr_values[i];
        }
        else {
            p_fp_info->edch_type = 0;
        }
    }

    /* Interface must be IuB */
    p_fp_info->iface_type = IuB_Interface;

    /* Store info in packet */
    p_add_proto_data(wmem_file_scope(), pinfo, proto_fp, 0, p_fp_info);
}