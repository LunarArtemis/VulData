static gboolean vwr_read_s2_s3_W_rec(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                     Buffer *buf, const guint8 *rec, int rec_size,
                                     int IS_TX, int *err, gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                   /* bytes output to buf so far */
    register int     i;                                   /* temps */
    const guint8     *s_start_ptr,*s_trail_ptr, *plcp_ptr, *m_ptr; /* stats & MPDU ptr */
    guint32          msdu_length, actual_octets;          /* octets in frame */
    guint8           l1p_1,l1p_2, plcp_type, mcs_index, nss;   /* mod (CCK-L/CCK-S/OFDM) */
    guint            flow_seq;
    guint64          s_time = LL_ZERO, e_time = LL_ZERO;  /* start/end */
                                                          /*  times, nsec */
    guint64          latency = LL_ZERO;
    guint64          start_time, s_sec, s_usec = LL_ZERO; /* start time, sec + usec */
    guint64          end_time;                            /* end time */
    guint16          info;                                /* INFO/ERRORS fields in stats blk */
    guint32          errors;
    gint8            rssi[] = {0,0,0,0};                  /* RSSI, signed 8-bit number */
    int              f_tx;                                /* flag: if set, is a TX frame */
    guint16          vc_id, ht_len=0;                     /* VC ID , total ip length*/
    guint32          flow_id, d_time;                     /* flow ID, packet duration*/
    int              sig_off, pay_off;                    /* MAC+SNAP header len, signature offset */
    guint64          sig_ts, tsid;                        /* 32 LSBs of timestamp in signature */
    guint16          chanflags = 0;                       /* channel flags for WLAN metadata header */
    guint16          radioflags = 0;                      /* flags for WLAN metadata header */
    guint64          delta_b;                             /* Used for calculating latency */
    guint16          phyRate;
    guint16          vw_flags;                            /* VeriWave-specific packet flags */
    guint8           vht_ndp_flag = 0;

    /*
     * The record data must be large enough to hold the statistics header,
     * the PLCP, and the statistics trailer.
     */
    if ((guint)rec_size < vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                    rec_size,
                                    vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /* Calculate the start of the statistics blocks in the buffer */
    /* Also get a bunch of fields from the stats blocks */
    s_start_ptr = &(rec[0]);                              /* point to stats header */
    s_trail_ptr = &(rec[rec_size - vVW510021_W_STATS_TRAILER_LEN]);      /* point to stats trailer */

    /* L1p info is different for series III and for Series II - need to check */
    l1p_1 = s_start_ptr[vVW510021_W_L1P_1_OFF];
    l1p_2 = s_start_ptr[vVW510021_W_L1P_2_OFF];
    if (vwr->FPGA_VERSION == S2_W_FPGA)
    {
        mcs_index = vVW510021_W_S2_MCS_INDEX(l1p_1);
        plcp_type = vVW510021_W_S2_PLCP_TYPE(l1p_2);
        /* we do the range checks at the end before copying the values
           into the wtap header */
        msdu_length = ((s_start_ptr[vVW510021_W_MSDU_LENGTH_OFF+1] & 0x1f) << 8)
                        + s_start_ptr[vVW510021_W_MSDU_LENGTH_OFF];

        vc_id = pntoh16(&s_start_ptr[vVW510021_W_VCID_OFF]);
        if (IS_TX)
        {
            rssi[0] = (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x80) ?
                       -1 * (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x7f) :
                       s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x7f;
        }
        else
        {
            rssi[0] = (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x80) ?
                      (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF]- 256) :
                      s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF];
        }
        rssi[1] = 100;
        rssi[2] = 100;
        rssi[3] = 100;

        nss = 0;

        /* XXX - S2 claims to have 11 bytes of PLCP and 1 byte of pad */
        plcp_ptr = &(rec[8]);
    }
    else
    {
        /* XXX - what indicates which packets include the FCS? */
        radioflags |= FLAGS_FCS;
        plcp_type = vVW510021_W_S3_PLCP_TYPE(l1p_2);
        if (plcp_type == vVW510021_W_PLCP_VHT_MIXED)
        {
            /* VHT */
            mcs_index = vVW510021_W_S3_MCS_INDEX_VHT(l1p_1);
            nss = vVW510021_W_S3_NSS_VHT(l1p_1);
        }
        else
        {
            /* HT */
            mcs_index = vVW510021_W_S3_MCS_INDEX_HT(l1p_1);
            nss = 0;
        }

        /*** Extract NDP Flag if it is a received frame ***/
        if (!IS_TX){
            vht_ndp_flag = s_start_ptr[8];
        }

        msdu_length = pntoh24(&s_start_ptr[9]);
        vc_id = pntoh16(&s_start_ptr[14]) & vVW510024_W_VCID_MASK;
        for (i = 0; i < 4; i++)
        {
            if (IS_TX)
            {
                rssi[i] = (s_start_ptr[4+i] & 0x80) ? -1 * (s_start_ptr[4+i] & 0x7f) : s_start_ptr[4+i] & 0x7f;
            }
            else
            {
                rssi[i] = (s_start_ptr[4+i] >= 128) ? (s_start_ptr[4+i] - 256) : s_start_ptr[4+i];
            }
        }

        /*** 16 bytes of PLCP header + 1 byte of L1P for user position ***/
        /* XXX - S3 claims to have 16 bytes of stats block and 16 bytes of
           *something*. Are those 16 bytes the PLCP? */
        plcp_ptr = &(rec[16]);
    }
    actual_octets = msdu_length;

    /*
     * Sanity check the octets field to determine if it's greater than
     * the packet data available in the record - i.e., the record size
     * minus the sum of (length of statistics header + PLCP) and
     * (length of statistics trailer).
     *
     * Report an error if it is.
     */
    if (actual_octets > rec_size - (vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN)) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    f_tx = IS_TX;
    flow_seq = s_trail_ptr[vVW510021_W_FLOWSEQ_OFF];

    latency = 0x00000000;                        /* clear latency */
    flow_id = pntoh24(&s_trail_ptr[vVW510021_W_FLOWID_OFF]);         /* all 24 bits valid */
    /* For tx latency is duration, for rx latency is timestamp */
    /* Get 48-bit latency value */
    tsid = pcorey48tohll(&s_trail_ptr[vVW510021_W_LATVAL_OFF]);

    errors = pntoh32(&s_trail_ptr[vVW510021_W_ERRORS_OFF]);
    info = pntoh16(&s_trail_ptr[vVW510021_W_INFO_OFF]);
    if ((info & v22_W_AGGREGATE_FLAGS) != 0)
    /* this length includes the Start_Spacing + Delimiter + MPDU + Padding for each piece of the aggregate*/
        ht_len = pletoh16(&s_start_ptr[vwr->PLCP_LENGTH_OFF]);


    /* decode OFDM or CCK PLCP header and determine rate and short preamble flag */
    /* the SIGNAL byte is always the first byte of the PLCP header in the frame */
    if (plcp_type == vVW510021_W_PLCP_LEGACY){
        /*
         * From IEEE Std 802.11-2012:
         *
         * According to section 17.2.2 "PPDU format", the PLCP header
         * for the High Rate DSSS PHY (11b) has a SIGNAL field that's
         * 8 bits, followed by a SERVICE field that's 8 bits, followed
         * by a LENGTH field that's 16 bits, followed by a CRC field
         * that's 16 bits.  The PSDU follows it.  Section 17.2.3 "PPDU
         * field definitions" describes those fields.
         *
         * According to sections 18.3.2 "PLCP frame format" and 18.3.4
         * "SIGNAL field", the PLCP for the OFDM PHY (11a) has a SIGNAL
         * field that's 24 bits, followed by a service field that's
         * 16 bits, followed by the PSDU.  Section 18.3.5.2 "SERVICE
         * field" describes the SERVICE field.
         *
         * According to section 19.3.2 "PPDU format", the frames for the
         * Extended Rate PHY (11g) either extend the 11b format, using
         * additional bits in the SERVICE field, or extend the 11a
         * format.
         */
        if (mcs_index < 4) {
            chanflags |= CHAN_CCK;
        }
        else {
            chanflags |= CHAN_OFDM;
        }
    }
    else if (plcp_type == vVW510021_W_PLCP_MIXED) {
        /*
         * According to section 20.3.2 "PPDU format", the HT-mixed
         * PLCP header has a "Non-HT SIGNAL field" (L-SIG), which
         * looks like an 11a SIGNAL field, followed by an HT SIGNAL
         * field (HT-SIG) described in section 20.3.9.4.3 "HT-SIG
         * definition".
         *
         * This means that the first octet of HT-SIG is at
         * plcp_ptr[3], skipping the 3 octets of the L-SIG field.
         *
         * 0x80 is the CBW 20/40 bit of HT-SIG.
         */
        /* set the appropriate flags to indicate HT mode and CB */
        radioflags |= FLAGS_CHAN_HT | ((plcp_ptr[3] & 0x80) ? FLAGS_CHAN_40MHZ : 0) |
                      ((l1p_1 & vVW510021_W_IS_LONGGI) ? 0 : FLAGS_CHAN_SHORTGI);
        chanflags  |= CHAN_OFDM;
    }
    else if (plcp_type == vVW510021_W_PLCP_GREENFIELD) {
        /*
         * According to section 20.3.2 "PPDU format", the HT-greenfield
         * PLCP header just has the HT SIGNAL field (HT-SIG) above, with
         * no L-SIG field.
         *
         * This means that the first octet of HT-SIG is at
         * plcp_ptr[0], as there's no L-SIG field to skip.
         *
         * 0x80 is the CBW 20/40 bit of HT-SIG.
         */
        /* set the appropriate flags to indicate HT mode and CB */
        radioflags |= FLAGS_CHAN_HT | ((plcp_ptr[0] & 0x80) ? FLAGS_CHAN_40MHZ : 0) |
                      ((l1p_1 & vVW510021_W_IS_LONGGI) ?  0 : FLAGS_CHAN_SHORTGI);
        chanflags  |= CHAN_OFDM;
    }
    else if (plcp_type == vVW510021_W_PLCP_VHT_MIXED) {
        /*
         * According to section 22.3.2 "VHTPPDU format" of IEEE Std
         * 802.11ac-2013, the VHT PLCP header has a "non-HT SIGNAL field"
         * (L-SIG), which looks like an 11a SIGNAL field, followed by
         * a VHT Signal A field (VHT-SIG-A) described in section
         * 22.3.8.3.3 "VHT-SIG-A definition", with training fields
         * between it and a VHT Signal B field (VHT-SIG-B) described
         * in section 22.3.8.3.6 "VHT-SIG-B definition", followed by
         * the PSDU.
         */
        guint8 SBW = vVW510021_W_BANDWIDTH_VHT(l1p_2);
        radioflags |= FLAGS_CHAN_VHT | ((l1p_1 & vVW510021_W_IS_LONGGI) ?  0 : FLAGS_CHAN_SHORTGI);
        chanflags |= CHAN_OFDM;
        if (SBW == 3)
            radioflags |= FLAGS_CHAN_40MHZ;
        else if (SBW == 4)
            radioflags |= FLAGS_CHAN_80MHZ;
    }

    /* Calculate start & end times (in sec/usec), converting 64-bit times to usec. */
    /* 64-bit times are "Corey-endian" */
    s_time = pcoreytohll(&s_trail_ptr[vVW510021_W_STARTT_OFF]);
    e_time = pcoreytohll(&s_trail_ptr[vVW510021_W_ENDT_OFF]);

    /* find the packet duration (difference between start and end times) */
    d_time = (guint32)((e_time - s_time) / NS_IN_US);  /* find diff, converting to usec */

    /* also convert the packet start time to seconds and microseconds */
    start_time = s_time / NS_IN_US;                     /* convert to microseconds first */
    s_sec = (start_time / US_IN_SEC);                   /* get the number of seconds */
    s_usec = start_time - (s_sec * US_IN_SEC);          /* get the number of microseconds */

    /* also convert the packet end time to seconds and microseconds */
    end_time = e_time / NS_IN_US;                       /* convert to microseconds first */

    /* extract the 32 LSBs of the signature timestamp field */
    m_ptr = &(rec[8+12]);
    pay_off = 42;         /* 24 (MAC) + 8 (SNAP) + IP */
    sig_off = find_signature(m_ptr, rec_size - 20, pay_off, flow_id, flow_seq);
    if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= (rec_size - vVW510021_W_STATS_TRAILER_LEN)))
        sig_ts = get_signature_ts(m_ptr, sig_off);
    else
        sig_ts = 0;

    /* Set latency based on rx/tx and signature timestamp */
    if (!IS_TX) {
        if (tsid < s_time) {
            latency = s_time - tsid;
          } else {
            /* Account for the rollover case. Since we cannot use 0x100000000 - l_time + s_time */
            /* we look for a large difference between l_time and s_time. */
            delta_b = tsid - s_time;
            if (delta_b >  0x10000000)
              latency = 0;
            else
              latency = delta_b;
          }
    }

    /*
     * Fill up the per-packet header.
     *
     * We also copy over 16 bytes of PLCP header + 1 byte of L1P for user
     * position.
     *
     * XXX - for S2, we don't have 16 bytes of PLCP header; do we have
     * the 1 byte of L1P?  The current Veriwave dissector just blindly
     * assumes there's a 17-byte blob before the 802.11 header.
     *
     * We include the length of the metadata headers in the packet lengths.
     */
    phdr->len = STATS_COMMON_FIELDS_LEN + EXT_WLAN_FIELDS_LEN + 1 + 16 + actual_octets;
    phdr->caplen = STATS_COMMON_FIELDS_LEN + EXT_WLAN_FIELDS_LEN + 1 + 16 + actual_octets;
    if (phdr->caplen > WTAP_MAX_PACKET_SIZE) {
        /*
         * Probably a corrupt capture file; return an error,
         * so that our caller doesn't blow up trying to allocate
         * space for an immensely-large packet.
         */
        *err_info = g_strdup_printf("vwr: File has %u-byte packet, bigger than maximum of %u",
                                    phdr->caplen, WTAP_MAX_PACKET_SIZE);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    phdr->ts.secs   = (time_t)s_sec;
    phdr->ts.nsecs  = (int)(s_usec * 1000);
    phdr->pkt_encap = WTAP_ENCAP_IXVERIWAVE;

    phdr->rec_type = REC_TYPE_PACKET;
    phdr->presence_flags = WTAP_HAS_TS;

    ws_buffer_assure_space(buf, phdr->caplen);
    data_ptr = ws_buffer_start_ptr(buf);

    /*
     * Generate and copy out the common metadata headers,
     * set the port type to 0 (WLAN).
     *
     * All values are copied out in little-endian byte order.
     */
    /*** msdu_length = msdu_length + 16; ***/
    phtoles(&data_ptr[bytes_written], 0); /* port_type */
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], STATS_COMMON_FIELDS_LEN); /* it_len */
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], msdu_length);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], flow_id);
    bytes_written += 4;
    phtoles(&data_ptr[bytes_written], vc_id);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], flow_seq);
    bytes_written += 2;
    if (!f_tx && sig_ts != 0) {
        phtolel(&data_ptr[bytes_written], latency);
    } else {
        phtolel(&data_ptr[bytes_written], 0);
    }
    bytes_written += 4;
    phtolel(&data_ptr[bytes_written], sig_ts); /* 32 LSBs of signature timestamp (nsec) */
    bytes_written += 4;
    phtolell(&data_ptr[bytes_written], start_time); /* record start & end times of frame */
    bytes_written += 8;
    phtolell(&data_ptr[bytes_written], end_time);
    bytes_written += 8;
    phtolel(&data_ptr[bytes_written], d_time);
    bytes_written += 4;

    /*
     * Generate and copy out the WLAN metadata headers.
     *
     * All values are copied out in little-endian byte order.
     */
    phtoles(&data_ptr[bytes_written], EXT_WLAN_FIELDS_LEN);
    bytes_written += 2;
    if (info & vVW510021_W_IS_WEP)
        radioflags |= FLAGS_WEP;
    if (!(l1p_1 & vVW510021_W_IS_LONGPREAMBLE) && (plcp_type == vVW510021_W_PLCP_LEGACY))
        radioflags |= FLAGS_SHORTPRE;
    phtoles(&data_ptr[bytes_written], radioflags);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], chanflags);
    bytes_written += 2;
    phyRate = (guint16)(getRate(plcp_type, mcs_index, radioflags, nss) * 10);
    phtoles(&data_ptr[bytes_written], phyRate);
    bytes_written += 2;

    /*** If received frame populate the ndp_flag in the same byte as plcp_type***/

    if (!IS_TX) {
        plcp_type = vht_ndp_flag  + plcp_type;
    }

    data_ptr[bytes_written] = plcp_type;
    bytes_written += 1;

    data_ptr[bytes_written] = mcs_index;
    bytes_written += 1;

    data_ptr[bytes_written] = nss;
    bytes_written += 1;
    data_ptr[bytes_written] = rssi[0];
    bytes_written += 1;
    data_ptr[bytes_written] = rssi[1];
    bytes_written += 1;
    data_ptr[bytes_written] = rssi[2];
    bytes_written += 1;
    data_ptr[bytes_written] = rssi[3];
    bytes_written += 1;
    /* padding */
    data_ptr[bytes_written] = 0;
    bytes_written += 1;

    /* fill in the VeriWave flags field */
    vw_flags  = 0;
    if (f_tx)
        vw_flags |= VW_FLAGS_TXF;
    if (errors & 0x1f)  /* If any error is flagged, then set the FCS error bit */
        vw_flags |= VW_FLAGS_FCSERR;
    if (!f_tx && (errors & vwr->CRYPTO_ERR))
        vw_flags |= VW_FLAGS_DCRERR;
    if (!f_tx && (errors & vwr->RETRY_ERR))
        vw_flags |= VW_FLAGS_RETRERR;
    if (info & vwr->WEPTYPE)
        vw_flags |= VW_FLAGS_IS_WEP;
    else if (info & vwr->TKIPTYPE)
        vw_flags |= VW_FLAGS_IS_TKIP;
    else if (info & vwr->CCMPTYPE)
        vw_flags |= VW_FLAGS_IS_CCMP;
    phtoles(&data_ptr[bytes_written], vw_flags);
    bytes_written += 2;

    phtoles(&data_ptr[bytes_written], ht_len);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], info);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], errors);
    bytes_written += 4;

    /*
     * XXX - is this supposed to be the RX L1 info, i.e. the "1 byte of L1P
     * for user position"?
     */
    if (!IS_TX){
      data_ptr[bytes_written] = vht_ndp_flag;
    } else {
      data_ptr[bytes_written] = 0;
    }
    bytes_written += 1;

    /*
     * Copy PLCP header.
     *
     * XXX - shouldn't that use plcp_ptr?
     *
     * XXX - what about S2, where we don't have 16 bytes of PLCP?
     */
    memcpy(&data_ptr[bytes_written], &rec[16], 16);
    bytes_written += 16;

    /*
     * Finally, copy the whole MAC frame to the packet buffer as-is.
     * This does not include the stats header or the PLCP header.
     */
    memcpy(&data_ptr[bytes_written], &rec[vwr->MPDU_OFF], actual_octets);

    return TRUE;
}