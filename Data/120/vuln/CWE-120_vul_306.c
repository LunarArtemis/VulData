static gint
dissect_bthci_iso(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    proto_item               *bthci_iso_item;
    proto_tree               *bthci_iso_tree;
    proto_item               *sub_item;
    guint16                   flags;
    guint16                   pb_flag = FALSE;
    guint16                   length;
    gboolean                  fragmented = FALSE;
    gint                      offset = 0;
    tvbuff_t                 *next_tvb;
    chandle_data_t           *chandle_data;
    bluetooth_data_t         *bluetooth_data;
    wmem_tree_key_t           key[6];
    guint32                   k_connection_handle = 0;
    guint32                   k_stream_handle;
    guint32                   k_frame_number;
    guint32                   k_interface_id;
    guint32                   k_adapter_id;
    guint32                   direction;
    remote_bdaddr_t          *remote_bdaddr;
    const gchar              *localhost_name;
    guint8                   *localhost_bdaddr;
    const gchar              *localhost_ether_addr;
    gchar                    *localhost_addr_name;
    gint                      localhost_length;
    localhost_bdaddr_entry_t *localhost_bdaddr_entry;
    localhost_name_entry_t   *localhost_name_entry;
    chandle_session_t        *chandle_session;
    wmem_tree_t              *subtree;
    stream_connection_handle_pair_t *handle_pairs;
    iso_data_info_t          iso_data_info;

    /* Reject the packet if data is NULL */
    if (data == NULL)
        return 0;

    bthci_iso_item = proto_tree_add_item(tree, proto_bthci_iso, tvb, offset, -1, ENC_NA);
    bthci_iso_tree = proto_item_add_subtree(bthci_iso_item, ett_bthci_iso);

    switch (pinfo->p2p_dir) {
        case P2P_DIR_SENT:
            col_set_str(pinfo->cinfo, COL_INFO, "Sent ");
            break;
        case P2P_DIR_RECV:
            col_set_str(pinfo->cinfo, COL_INFO, "Rcvd ");
            break;
        default:
            col_set_str(pinfo->cinfo, COL_INFO, "UnknownDirection ");
            break;
    }

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "HCI_ISO");

    flags   = tvb_get_letohs(tvb, offset);
    pb_flag = (flags >> 12) & 0x3;
    iso_data_info.timestamp_present = (flags >> 14) & 0x1;
    iso_data_info.handle = flags & 0xfff;
    proto_tree_add_item(bthci_iso_tree, hf_bthci_iso_chandle, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(bthci_iso_tree, hf_bthci_iso_pb_flag, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(bthci_iso_tree, hf_bthci_iso_ts_flag, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(bthci_iso_tree, hf_bthci_iso_reserved, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    offset += 2;

    length = tvb_get_letohs(tvb, offset);
    sub_item = proto_tree_add_item(bthci_iso_tree, hf_bthci_iso_data_length, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    offset += 2;

    /* determine if packet is fragmented */
    if (pb_flag != 0x2) {
        fragmented = TRUE;
    }

    bluetooth_data = (bluetooth_data_t *) data;
    DISSECTOR_ASSERT(bluetooth_data);

    k_interface_id      = bluetooth_data->interface_id;
    k_adapter_id        = bluetooth_data->adapter_id;
    k_stream_handle     = flags & 0x0fff;
    direction           = pinfo->p2p_dir;
    k_frame_number      = pinfo->num;

    key[0].length = 1;
    key[0].key    = &k_interface_id;
    key[1].length = 1;
    key[1].key    = &k_adapter_id;
    key[2].length = 1;
    key[2].key    = &k_stream_handle;
    key[3].length = 0;
    key[3].key    = NULL;

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->chandle_sessions, key);
    chandle_session = (subtree) ? (chandle_session_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (!(chandle_session &&
            chandle_session->connect_in_frame < pinfo->num &&
            chandle_session->disconnect_in_frame > pinfo->num)){
        chandle_session = NULL;
    }

    /* replace stream (CIS/BIS) handle with connection (ACL) handle */
    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->shandle_to_chandle, key);
    handle_pairs = (subtree) ? (stream_connection_handle_pair_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (handle_pairs) {
        k_connection_handle = handle_pairs->chandle;
        key[2].key = &k_connection_handle;
    }

    key[3].length = 1;
    key[3].key    = &k_frame_number;
    key[4].length = 0;
    key[4].key    = NULL;

    /* remote bdaddr and name */
    remote_bdaddr = (remote_bdaddr_t *)wmem_tree_lookup32_array_le(bluetooth_data->chandle_to_bdaddr, key);
    /*
     * XXX - do this only if we found a handle pair, so that we have
     * a connection handle?
     */
    if (remote_bdaddr && remote_bdaddr->interface_id == bluetooth_data->interface_id &&
            remote_bdaddr->adapter_id == bluetooth_data->adapter_id &&
            remote_bdaddr->chandle == k_connection_handle) {
        guint32         k_bd_addr_oui;
        guint32         k_bd_addr_id;
        guint32         bd_addr_oui;
        guint32         bd_addr_id;
        device_name_t  *device_name;
        const gchar    *remote_name;
        const gchar    *remote_ether_addr;
        gchar          *remote_addr_name;
        gint            remote_length;

        bd_addr_oui = remote_bdaddr->bd_addr[0] << 16 | remote_bdaddr->bd_addr[1] << 8 | remote_bdaddr->bd_addr[2];
        bd_addr_id  = remote_bdaddr->bd_addr[3] << 16 | remote_bdaddr->bd_addr[4] << 8 | remote_bdaddr->bd_addr[5];

        k_bd_addr_oui  = bd_addr_oui;
        k_bd_addr_id   = bd_addr_id;
        k_frame_number = pinfo->num;

        key[0].length = 1;
        key[0].key    = &k_interface_id;
        key[1].length = 1;
        key[1].key    = &k_adapter_id;
        key[2].length = 1;
        key[2].key    = &k_bd_addr_id;
        key[3].length = 1;
        key[3].key    = &k_bd_addr_oui;
        key[4].length = 1;
        key[4].key    = &k_frame_number;
        key[5].length = 0;
        key[5].key    = NULL;

        device_name = (device_name_t *)wmem_tree_lookup32_array_le(bluetooth_data->bdaddr_to_name, key);
        if (device_name && device_name->bd_addr_oui == bd_addr_oui && device_name->bd_addr_id == bd_addr_id)
            remote_name = device_name->name;
        else
            remote_name = "";

        remote_ether_addr = get_ether_name(remote_bdaddr->bd_addr);
        remote_length = (gint)(strlen(remote_ether_addr) + 3 + strlen(remote_name) + 1);
        remote_addr_name = (gchar *)wmem_alloc(pinfo->pool, remote_length);

        g_snprintf(remote_addr_name, remote_length, "%s (%s)", remote_ether_addr, remote_name);

        if (pinfo->p2p_dir == P2P_DIR_RECV) {
            set_address(&pinfo->net_src, AT_STRINGZ, (int)strlen(remote_name) + 1, remote_name);
            set_address(&pinfo->dl_src, AT_ETHER, 6, remote_bdaddr->bd_addr);
            set_address(&pinfo->src, AT_STRINGZ, (int)strlen(remote_addr_name) + 1, remote_addr_name);
        } else if (pinfo->p2p_dir == P2P_DIR_SENT) {
            set_address(&pinfo->net_dst, AT_STRINGZ, (int)strlen(remote_name) + 1, remote_name);
            set_address(&pinfo->dl_dst, AT_ETHER, 6, remote_bdaddr->bd_addr);
            set_address(&pinfo->dst, AT_STRINGZ, (int)strlen(remote_addr_name) + 1, remote_addr_name);
        }
    } else {
        if (pinfo->p2p_dir == P2P_DIR_RECV) {
            set_address(&pinfo->net_src, AT_STRINGZ, 1, "");
            set_address(&pinfo->dl_src, AT_STRINGZ, 1, "");
            set_address(&pinfo->src, AT_STRINGZ, 10, "remote ()");
        } else if (pinfo->p2p_dir == P2P_DIR_SENT) {
            set_address(&pinfo->net_dst, AT_STRINGZ, 1, "");
            set_address(&pinfo->dl_dst, AT_STRINGZ, 1, "");
            set_address(&pinfo->dst, AT_STRINGZ, 10, "remote ()");
        }
    }

    /* localhost bdaddr and name */
    key[0].length = 1;
    key[0].key    = &k_interface_id;
    key[1].length = 1;
    key[1].key    = &k_adapter_id;
    key[2].length = 1;
    key[2].key    = &k_frame_number;
    key[3].length = 0;
    key[3].key    = NULL;


    localhost_bdaddr_entry = (localhost_bdaddr_entry_t *)wmem_tree_lookup32_array_le(bluetooth_data->localhost_bdaddr, key);
    localhost_bdaddr = (guint8 *) wmem_alloc(pinfo->pool, 6);
    if (localhost_bdaddr_entry && localhost_bdaddr_entry->interface_id == bluetooth_data->interface_id &&
        localhost_bdaddr_entry->adapter_id == bluetooth_data->adapter_id) {

        localhost_ether_addr = get_ether_name(localhost_bdaddr_entry->bd_addr);
        memcpy(localhost_bdaddr, localhost_bdaddr_entry->bd_addr, 6);
    } else {
        localhost_ether_addr = "localhost";
        /* XXX - is this the right value to use? */
        memset(localhost_bdaddr, 0, 6);
    }

    localhost_name_entry = (localhost_name_entry_t *)wmem_tree_lookup32_array_le(bluetooth_data->localhost_name, key);
    if (localhost_name_entry && localhost_name_entry->interface_id == bluetooth_data->interface_id &&
            localhost_name_entry->adapter_id == bluetooth_data->adapter_id)
        localhost_name = localhost_name_entry->name;
    else
        localhost_name = "";

    localhost_length = (gint)(strlen(localhost_ether_addr) + 3 + strlen(localhost_name) + 1);
    localhost_addr_name = (gchar *)wmem_alloc(pinfo->pool, localhost_length);

    g_snprintf(localhost_addr_name, localhost_length, "%s (%s)", localhost_ether_addr, localhost_name);

    if (pinfo->p2p_dir == P2P_DIR_RECV) {
        set_address(&pinfo->net_dst, AT_STRINGZ, (int)strlen(localhost_name) + 1, localhost_name);
        set_address(&pinfo->dl_dst, AT_ETHER, 6, localhost_bdaddr);
        set_address(&pinfo->dst, AT_STRINGZ, (int)strlen(localhost_addr_name) + 1, localhost_addr_name);
    } else if (pinfo->p2p_dir == P2P_DIR_SENT) {
        set_address(&pinfo->net_src, AT_STRINGZ, (int)strlen(localhost_name) + 1, localhost_name);
        set_address(&pinfo->dl_src, AT_ETHER, 6, localhost_bdaddr);
        set_address(&pinfo->src, AT_STRINGZ, (int)strlen(localhost_addr_name) + 1, localhost_addr_name);
    }

    /* find the chandle_data structure associated with this chandle */
    key[0].length = 1;
    key[0].key = &k_interface_id;
    key[1].length = 1;
    key[1].key = &k_adapter_id;
    key[2].length = 1;
    key[2].key = &k_stream_handle;
    key[3].length = 1;
    key[3].key = &direction;
    key[4].length = 0;
    key[4].key = NULL;

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(chandle_tree, key);
    chandle_data = (subtree) ? (chandle_data_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (!pinfo->fd->visited && !chandle_data) {
        key[0].length = 1;
        key[0].key = &k_interface_id;
        key[1].length = 1;
        key[1].key = &k_adapter_id;
        key[2].length = 1;
        key[2].key = &k_stream_handle;
        key[3].length = 1;
        key[3].key = &direction;
        key[4].length = 1;
        key[4].key = &k_frame_number;
        key[5].length = 0;
        key[5].key = NULL;

        chandle_data = wmem_new(wmem_file_scope(), chandle_data_t);
        chandle_data->start_fragments = wmem_tree_new(wmem_file_scope());

        wmem_tree_insert32_array(chandle_tree, key, chandle_data);
    } else if (pinfo->fd->visited && !chandle_data) {
        DISSECTOR_ASSERT_HINT(0, "Impossible: no previously session saved");
    }

    if (!fragmented || (!iso_reassembly && !pb_flag)) {
        /* call ISO data dissector for PDUs that are not fragmented
         * also for the first fragment if reassembly is disabled
         */
        if (length < tvb_captured_length_remaining(tvb, offset)) {
            if (!fragmented)
                expert_add_info(pinfo, sub_item, &ei_length_bad);
            /* Try to dissect as much as possible */
            length = tvb_captured_length_remaining(tvb, offset);
        }

        next_tvb = tvb_new_subset_length_caplen(tvb, offset, tvb_captured_length_remaining(tvb, offset), length);
        call_dissector_with_data(bthci_iso_data_handle, next_tvb, pinfo, tree, &iso_data_info);
    } else if (fragmented && iso_reassembly) {
        multi_fragment_pdu_t *mfp = NULL;
        gint                  len;
        if (pb_flag == 0x00) { /* first fragment */
            if (!pinfo->fd->visited) {
                gint timestamp_size = 0;
                mfp = (multi_fragment_pdu_t *) wmem_new(wmem_file_scope(), multi_fragment_pdu_t);
                mfp->first_frame = pinfo->num;
                mfp->last_frame  = 0;
                mfp->tot_len     = 4;
                len = tvb_captured_length_remaining(tvb, offset);
                if (flags & 0x4000) { /* 4 byte timestamp is present */
                    timestamp_size = 4;
                }
                mfp->tot_len += timestamp_size + (tvb_get_letohs(tvb, offset + 2 + timestamp_size) & 0xfff);
                mfp->reassembled = (char *) wmem_alloc(wmem_file_scope(), mfp->tot_len);
                if (len <= mfp->tot_len) {
                    tvb_memcpy(tvb, (guint8 *) mfp->reassembled, offset, len);
                    mfp->cur_off = len;
                    wmem_tree_insert32(chandle_data->start_fragments, pinfo->num, mfp);
                }
            } else {
                mfp = (multi_fragment_pdu_t *)wmem_tree_lookup32(chandle_data->start_fragments, pinfo->num);
            }
            if (mfp != NULL && mfp->last_frame) {
                proto_item *item;

                item = proto_tree_add_uint(bthci_iso_tree, hf_bthci_iso_reassembled_in, tvb, 0, 0, mfp->last_frame);
                proto_item_set_generated(item);
                col_append_frame_number(pinfo, COL_INFO, " [Reassembled in #%u]", mfp->last_frame);
            }
        }
        else if (pb_flag & 0x01) { /* continuation/last fragment */
            mfp = (multi_fragment_pdu_t *)wmem_tree_lookup32_le(chandle_data->start_fragments, pinfo->num);
            if (!pinfo->fd->visited) {
                len = tvb_captured_length_remaining(tvb, offset);
                if (mfp != NULL && !mfp->last_frame) {
                    tvb_memcpy(tvb, (guint8 *) mfp->reassembled + mfp->cur_off, offset, len);
                    mfp->cur_off += len;
                    if (pb_flag == 0x03)
                        mfp->last_frame = pinfo->num;
                }
            }
            if (mfp) {
                proto_item *item;

                item = proto_tree_add_uint(bthci_iso_tree, hf_bthci_iso_continuation_to, tvb, 0, 0, mfp->first_frame);
                proto_item_set_generated(item);
                col_append_frame_number(pinfo, COL_INFO, " [Continuation to #%u]", mfp->first_frame);
                if (mfp->last_frame && mfp->last_frame != pinfo->num) {
                    item = proto_tree_add_uint(bthci_iso_tree, hf_bthci_iso_reassembled_in, tvb, 0, 0, mfp->last_frame);
                    proto_item_set_generated(item);
                    col_append_frame_number(pinfo, COL_INFO, " [Reassembled in #%u]", mfp->last_frame);
                }

                if (pb_flag == 0x03) { /* last fragment */
                    next_tvb = tvb_new_child_real_data(tvb, (guint8 *) mfp->reassembled, mfp->tot_len, mfp->tot_len);
                    add_new_data_source(pinfo, next_tvb, "Reassembled BTHCI ISO");

                    call_dissector_with_data(bthci_iso_data_handle, next_tvb, pinfo, tree, &iso_data_info);
                }
            }
        }
    }

    if (tvb_captured_length_remaining(tvb, offset) > 0) {
        sub_item = proto_tree_add_item(bthci_iso_tree, hf_bthci_iso_data, tvb, offset, -1, ENC_NA);
        if (fragmented) {
            proto_item_append_text(sub_item, " Fragment");
        }
    }

    if (chandle_session) {
        sub_item = proto_tree_add_uint(bthci_iso_tree, hf_bthci_iso_connect_in, tvb, 0, 0, chandle_session->connect_in_frame);
        proto_item_set_generated(sub_item);

        if (chandle_session->disconnect_in_frame < G_MAXUINT32) {
            sub_item = proto_tree_add_uint(bthci_iso_tree, hf_bthci_iso_disconnect_in, tvb, 0, 0, chandle_session->disconnect_in_frame);
            proto_item_set_generated(sub_item);
        }
    }

    return tvb_reported_length(tvb);
}