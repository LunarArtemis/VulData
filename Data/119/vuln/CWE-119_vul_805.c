static gboolean vwr_read_s1_W_rec(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                  Buffer *buf, const guint8 *rec, int rec_size,
                                  int *err, gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                   /* bytes output to buf so far */
    const guint8     *s_ptr, *m_ptr;                      /* stats pointer */
    guint16          msdu_length, actual_octets;          /* octets in frame */
    guint16          plcp_hdr_len;                        /* PLCP header length */
    guint16          rflags;
    guint8           m_type;                              /* mod type (CCK-L/CCK-S/OFDM), seqnum */
    guint            flow_seq;
    guint64          s_time = LL_ZERO, e_time = LL_ZERO;  /* start/end */
                                                          /* times, nsec */
    guint32          latency;
    guint64          start_time, s_sec, s_usec = LL_ZERO; /* start time, sec + usec */
    guint64          end_time;                            /* end time */
    guint32          info;                                /* INFO/ERRORS fields in stats blk */
    gint8            rssi;                                /* RSSI, signed 8-bit number */
    int              f_tx;                                /* flag: if set, is a TX frame */
    guint8           plcp_type, mcs_index, nss;           /* PLCP type 0: Legacy, 1: Mixed, 2: Green field, 3: VHT Mixed */
    guint16          vc_id, ht_len=0;                     /* VC ID, total ip length */
    guint            flow_id;                             /* flow ID */
    guint32          d_time, errors;                      /* packet duration & errors */
    int              sig_off, pay_off;                    /* MAC+SNAP header len, signature offset */
    guint64          sig_ts;                              /* 32 LSBs of timestamp in signature */
    guint16          phyRate;
    guint16          vw_flags;                            /* VeriWave-specific packet flags */

    /*
     * The record data must be large enough to hold the statistics trailer.
     */
    if (rec_size < v22_W_STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                    rec_size, v22_W_STATS_LEN);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /* Calculate the start of the statistics block in the buffer */
    /* Also get a bunch of fields from the stats block */
    s_ptr    = &(rec[rec_size - v22_W_STATS_LEN]); /* point to it */
    m_type   = s_ptr[v22_W_MTYPE_OFF] & 0x7;
    f_tx     = !(s_ptr[v22_W_MTYPE_OFF] & 0x8);
    actual_octets   = pntoh16(&s_ptr[v22_W_OCTET_OFF]);
    vc_id    = pntoh16(&s_ptr[v22_W_VCID_OFF]) & 0x3ff;
    flow_seq = s_ptr[v22_W_FLOWSEQ_OFF];

    latency = (guint32)pcorey48tohll(&s_ptr[v22_W_LATVAL_OFF]);

    flow_id = pntoh16(&s_ptr[v22_W_FLOWID_OFF+1]);  /* only 16 LSBs kept */
    errors  = pntoh16(&s_ptr[v22_W_ERRORS_OFF]);

    info = pntoh16(&s_ptr[v22_W_INFO_OFF]);
    rssi = (s_ptr[v22_W_RSSI_OFF] & 0x80) ? (-1 * (s_ptr[v22_W_RSSI_OFF] & 0x7f)) : s_ptr[v22_W_RSSI_OFF];

    /*
     * Sanity check the octets field to determine if it's greater than
     * the packet data available in the record - i.e., the record size
     * minus the length of the statistics block.
     *
     * Report an error if it is.
     */
    if (actual_octets > rec_size - v22_W_STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /* Decode OFDM or CCK PLCP header and determine rate and short preamble flag. */
    /* The SIGNAL byte is always the first byte of the PLCP header in the frame.  */
    plcp_type = 0;
    nss = 1;
    if (m_type == vwr->MT_OFDM)
        mcs_index = get_ofdm_rate(rec);
    else if ((m_type == vwr->MT_CCKL) || (m_type == vwr->MT_CCKS))
        mcs_index = get_cck_rate(rec);
    else
        mcs_index = 1;
    rflags  = (m_type == vwr->MT_CCKS) ? FLAGS_SHORTPRE : 0;
    /* Calculate the MPDU size/ptr stuff; MPDU starts at 4 or 6 depending on OFDM/CCK. */
    /* Note that the number of octets in the frame also varies depending on OFDM/CCK,  */
    /*  because the PLCP header is prepended to the actual MPDU.                       */
    plcp_hdr_len = (m_type == vwr->MT_OFDM) ? 4 : 6;
    if (actual_octets >= plcp_hdr_len)
       actual_octets -= plcp_hdr_len;
    else {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (too short to include %u-byte PLCP header)",
                                    actual_octets, plcp_hdr_len);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }
    m_ptr = &rec[plcp_hdr_len];
    msdu_length = actual_octets;

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
        *err_info = g_strdup_printf("vwr: Invalid data length %u (too short to include %u-byte PLCP header and 4 bytes of FCS)",
                                    actual_octets, plcp_hdr_len);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }
    actual_octets -= 4;

    /* Calculate start & end times (in sec/usec), converting 64-bit times to usec. */
    /* 64-bit times are "Corey-endian" */
    s_time = pcoreytohll(&s_ptr[v22_W_STARTT_OFF]);
    e_time = pcoreytohll(&s_ptr[v22_W_ENDT_OFF]);

    /* find the packet duration (difference between start and end times) */
    d_time = (guint32)((e_time - s_time) / NS_IN_US);   /* find diff, converting to usec */

    /* also convert the packet start time to seconds and microseconds */
    start_time = s_time / NS_IN_US;                /* convert to microseconds first  */
    s_sec      = (start_time / US_IN_SEC);         /* get the number of seconds      */
    s_usec     = start_time - (s_sec * US_IN_SEC); /* get the number of microseconds */

    /* also convert the packet end time to seconds and microseconds */
    end_time = e_time / NS_IN_US;                       /* convert to microseconds first */

    /* extract the 32 LSBs of the signature timestamp field from the data block*/
    pay_off = 42;    /* 24 (MAC) + 8 (SNAP) + IP */
    sig_off = find_signature(m_ptr, rec_size - 6, pay_off, flow_id, flow_seq);
    if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= (rec_size - v22_W_STATS_LEN)))
        sig_ts = get_signature_ts(m_ptr, sig_off);
    else
        sig_ts = 0;

    /*
     * Fill up the per-packet header.
     *
     * We also zero out 16 bytes PLCP header and 1 byte of L1P for user
     * position.
     *
     * XXX - for S1, do we even have that?  The current Veriwave dissector
     * just blindly assumes there's a 17-byte blob before the 802.11
     * header, which is why we fill in those extra zero bytes.
     *
     * We include the length of the metadata headers in the packet lengths.
     *
     * The maximum value of actual_octets is 65535, which, even after
     * adding the lengths of the metadata headers, is less than
     * WTAP_MAX_PACKET_SIZE will ever be, so we don't need to check it.
     */
    phdr->len = STATS_COMMON_FIELDS_LEN + EXT_WLAN_FIELDS_LEN + 1 + 16 + actual_octets;
    phdr->caplen = STATS_COMMON_FIELDS_LEN + EXT_WLAN_FIELDS_LEN + 1 + 16 + actual_octets;

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
    phtoles(&data_ptr[bytes_written], rflags);
    bytes_written += 2;
    if (m_type == vwr->MT_OFDM) {
        phtoles(&data_ptr[bytes_written], CHAN_OFDM);
    } else {
        phtoles(&data_ptr[bytes_written], CHAN_CCK);
    }
    bytes_written += 2;
    phyRate = (guint16)(getRate(plcp_type, mcs_index, rflags, nss) * 10);
    phtoles(&data_ptr[bytes_written], phyRate);
    bytes_written += 2;
    data_ptr[bytes_written] = plcp_type;
    bytes_written += 1;
    data_ptr[bytes_written] = mcs_index;
    bytes_written += 1;
    data_ptr[bytes_written] = nss;
    bytes_written += 1;
    data_ptr[bytes_written] = rssi;
    bytes_written += 1;
    /* antennae b, c, d signal power */
    data_ptr[bytes_written] = 100;
    bytes_written += 1;
    data_ptr[bytes_written] = 100;
    bytes_written += 1;
    data_ptr[bytes_written] = 100;
    bytes_written += 1;
    /* padding */
    data_ptr[bytes_written] = 0;
    bytes_written += 1;

    /* fill in the VeriWave flags field */
    vw_flags = 0;
    if (f_tx)
        vw_flags |= VW_FLAGS_TXF;
    if (errors & vwr->FCS_ERROR)
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
     * No VHT, no VHT NDP flag, so just zero.
     *
     * XXX - is this supposed to be the RX L1 info, i.e. the "1 byte of L1P
     * for user position"?
     */
    data_ptr[bytes_written] = 0;
    bytes_written += 1;

    /*
     * XXX - put a PLCP header here?  That's what's done for S3.
     */
    memset(&data_ptr[bytes_written], 0, 16);
    bytes_written += 16;

    /*
     * Finally, copy the whole MAC frame to the packet buffer as-is.
     * This does not include the PLCP; the MPDU starts at 4 or 6
     * depending on OFDM/CCK.
     * This also does not include the last 4 bytes, as those don't
     * contain an FCS, they just contain junk.
     */
    memcpy(&data_ptr[bytes_written], &rec[plcp_hdr_len], actual_octets);

    return TRUE;
}