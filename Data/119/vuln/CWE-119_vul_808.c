static gboolean vwr_read_rec_data_ethernet(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                           Buffer *buf, const guint8 *rec,
                                           int rec_size, int IS_TX, int *err,
                                           gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                   /* bytes output to buf so far */
    const guint8 *s_ptr, *m_ptr;                          /* stats and MPDU pointers */
    guint16          msdu_length, actual_octets;          /* octets in frame */
    guint            flow_seq;                            /* seqnum */
    guint64          s_time = LL_ZERO, e_time = LL_ZERO;  /* start/end */
                                                          /* times, nsec */
    guint32          latency = 0;
    guint64          start_time, s_sec, s_usec = LL_ZERO; /* start time, sec + usec */
    guint64          end_time;                            /* end time */
    guint            l4id;
    guint16          info, validityBits;                  /* INFO/ERRORS fields in stats */
    guint32          errors;
    guint16          vc_id;                               /* VC ID, total (incl of aggregates) */
    guint32          flow_id, d_time;                     /* packet duration */
    int              f_flow;                              /* flags: flow valid */
    guint32          frame_type;                          /* frame type field */
    int              mac_len, sig_off, pay_off;           /* MAC header len, signature offset */
    /* XXX - the code here fetched tsid, but never used it! */
    guint64          sig_ts/*, tsid*/;                    /* 32 LSBs of timestamp in signature */
    guint64          delta_b;                             /* Used for calculating latency */
    guint16          vw_flags;                            /* VeriWave-specific packet flags */

    if ((guint)rec_size < vwr->STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)", rec_size, vwr->STATS_LEN);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /* Calculate the start of the statistics block in the buffer. */
    /* Also get a bunch of fields from the stats block.           */
    m_ptr = &(rec[0]);                              /* point to the data block */
    s_ptr = &(rec[rec_size - vwr->STATS_LEN]);      /* point to the stats block */

    msdu_length = pntoh16(&s_ptr[vwr->OCTET_OFF]);
    actual_octets = msdu_length;

    /*
     * Sanity check the octets field to determine if it's greater than
     * the packet data available in the record - i.e., the record size
     * minus the length of the statistics block.
     *
     * Report an error if it is.
     */
    if (actual_octets > rec_size - vwr->STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }
    /*
     * The maximum value of actual_octets is 65535, which, even after
     * adding the lengths of the metadata headers, is less than
     * WTAP_MAX_PACKET_SIZE will ever be, so we don't need to check it.
     */

    vc_id = pntoh16(&s_ptr[vwr->VCID_OFF]) & vwr->VCID_MASK;
    flow_seq   = s_ptr[vwr->FLOWSEQ_OFF];
    frame_type = pntoh32(&s_ptr[vwr->FRAME_TYPE_OFF]);

    if (vwr->FPGA_VERSION == vVW510024_E_FPGA) {
        validityBits = pntoh16(&s_ptr[vwr->VALID_OFF]);
        f_flow = validityBits & vwr->FLOW_VALID;

        mac_len = (validityBits & vwr->IS_VLAN) ? 16 : 14;           /* MAC hdr length based on VLAN tag */


        errors = pntoh16(&s_ptr[vwr->ERRORS_OFF]);
    }
    else {
        f_flow  = s_ptr[vwr->VALID_OFF] & vwr->FLOW_VALID;
        mac_len = (frame_type & vwr->IS_VLAN) ? 16 : 14;             /* MAC hdr length based on VLAN tag */


        /* for older fpga errors is only represented by 16 bits) */
        errors = pntoh16(&s_ptr[vwr->ERRORS_OFF]);
    }

    info = pntoh16(&s_ptr[vwr->INFO_OFF]);
    /*  24 LSBs */
    flow_id = pntoh24(&s_ptr[vwr->FLOWID_OFF]);

#if 0
    /* For tx latency is duration, for rx latency is timestamp. */
    /* Get 64-bit latency value. */
    tsid = pcorey48tohll(&s_ptr[vwr->LATVAL_OFF]);
#endif

    l4id = pntoh16(&s_ptr[vwr->L4ID_OFF]);

    /*
     * The MSDU length includes the FCS.
     *
     * The packet data does *not* include the FCS - it's just 4 bytes
     * of junk - so we have to remove it.
     *
     * We'll be stripping off an FCS (?), so make sure we have at
     * least 4 octets worth of FCS.
     */
    if (actual_octets < 4) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (too short to include 4 bytes of FCS)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }
    actual_octets -= 4;

    /* Calculate start & end times (in sec/usec), converting 64-bit times to usec. */
    /* 64-bit times are "Corey-endian"                                             */
    s_time = pcoreytohll(&s_ptr[vwr->STARTT_OFF]);
    e_time = pcoreytohll(&s_ptr[vwr->ENDT_OFF]);

    /* find the packet duration (difference between start and end times) */
    d_time = (guint32)((e_time - s_time));  /* find diff, leaving in nsec for Ethernet */

    /* also convert the packet start time to seconds and microseconds */
    start_time = s_time / NS_IN_US;                     /* convert to microseconds first */
    s_sec = (start_time / US_IN_SEC);                   /* get the number of seconds */
    s_usec = start_time - (s_sec * US_IN_SEC);          /* get the number of microseconds */

    /* also convert the packet end time to seconds and microseconds */
    end_time = e_time / NS_IN_US;                       /* convert to microseconds first */

    if (frame_type & vwr->IS_TCP)                       /* signature offset for TCP frame */
    {
        pay_off = mac_len + 40;
    }
    else if (frame_type & vwr->IS_UDP)                  /* signature offset for UDP frame */
    {
        pay_off = mac_len + 28;
    }
    else if (frame_type & vwr->IS_ICMP)                 /* signature offset for ICMP frame */
    {
        pay_off = mac_len + 24;
    }
    else if (frame_type & vwr->IS_IGMP)                 /* signature offset for IGMPv2 frame */
    {
        pay_off = mac_len + 28;
    }
    else                                                /* signature offset for raw IP frame */
    {
        pay_off = mac_len + 20;
    }

    sig_off = find_signature(m_ptr, rec_size, pay_off, flow_id, flow_seq);
    if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= msdu_length) && (f_flow != 0))
        sig_ts = get_signature_ts(m_ptr, sig_off);
    else
        sig_ts = 0;

    /* Set latency based on rx/tx and signature timestamp */
    if (!IS_TX) {
        if (sig_ts < s_time) {
            latency = (guint32)(s_time - sig_ts);
        } else {
            /* Account for the rollover case. Since we cannot use 0x100000000 - l_time + s_time */
            /*  we look for a large difference between l_time and s_time.                       */
            delta_b = sig_ts - s_time;
            if (delta_b >  0x10000000) {
                latency = 0;
            } else
                latency = (guint32)delta_b;
        }
    }

    /*
     * Fill up the per-packet header.
     *
     * We include the length of the metadata headers in the packet lengths.
     */
    phdr->len = STATS_COMMON_FIELDS_LEN + EXT_ETHERNET_FIELDS_LEN + actual_octets;
    phdr->caplen = STATS_COMMON_FIELDS_LEN + EXT_ETHERNET_FIELDS_LEN + actual_octets;

    phdr->ts.secs   = (time_t)s_sec;
    phdr->ts.nsecs  = (int)(s_usec * 1000);
    phdr->pkt_encap = WTAP_ENCAP_IXVERIWAVE;

    phdr->rec_type = REC_TYPE_PACKET;
    phdr->presence_flags = WTAP_HAS_TS;

    /*etap_hdr.vw_ip_length = (guint16)ip_len;*/

    ws_buffer_assure_space(buf, phdr->caplen);
    data_ptr = ws_buffer_start_ptr(buf);

    /*
     * Generate and copy out the common metadata headers,
     * set the port type to 1 (Ethernet).
     *
     * All values are copied out in little-endian byte order.
     */
    phtoles(&data_ptr[bytes_written], 1);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], STATS_COMMON_FIELDS_LEN);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], msdu_length);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], flow_id);
    bytes_written += 4;
    phtoles(&data_ptr[bytes_written], vc_id);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], flow_seq);
    bytes_written += 2;
    if (!IS_TX && (sig_ts != 0)) {
        phtolel(&data_ptr[bytes_written], latency);
    } else {
        phtolel(&data_ptr[bytes_written], 0);
    }
    bytes_written += 4;
    phtolel(&data_ptr[bytes_written], sig_ts);
    bytes_written += 4;
    phtolell(&data_ptr[bytes_written], start_time)                  /* record start & end times of frame */
    bytes_written += 8;
    phtolell(&data_ptr[bytes_written], end_time);
    bytes_written += 8;
    phtolel(&data_ptr[bytes_written], d_time);
    bytes_written += 4;

    /*
     * Generate and copy out the Ethernet metadata headers.
     *
     * All values are copied out in little-endian byte order.
     */
    phtoles(&data_ptr[bytes_written], EXT_ETHERNET_FIELDS_LEN);
    bytes_written += 2;
    vw_flags = 0;
    if (IS_TX)
        vw_flags |= VW_FLAGS_TXF;
    if (errors & vwr->FCS_ERROR)
        vw_flags |= VW_FLAGS_FCSERR;
    phtoles(&data_ptr[bytes_written], vw_flags);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], info);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], errors);
    bytes_written += 4;
    phtolel(&data_ptr[bytes_written], l4id);
    bytes_written += 4;

    /* Add in pad */
    phtolel(&data_ptr[bytes_written], 0);
    bytes_written += 4;

    /*
     * Finally, copy the whole MAC frame to the packet buffer as-is.
     * This also does not include the last 4 bytes, as those don't
     * contain an FCS, they just contain junk.
     */
    memcpy(&data_ptr[bytes_written], m_ptr, actual_octets);

    return TRUE;
}