static int
dissect_pktc_rekey(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb, int offset, guint8 doi)
{
    guint32 snonce;
    guint string_len;
    const guint8 *timestr;

    /* Server Nonce */
    snonce=tvb_get_ntohl(tvb, offset);
    proto_tree_add_uint(tree, hf_pktc_server_nonce, tvb, offset, 4, snonce);
    offset+=4;

    /* Server Kerberos Principal Identifier */
    string_len=tvb_strsize(tvb, offset);
    proto_tree_add_item(tree, hf_pktc_server_principal, tvb, offset, string_len, ENC_ASCII|ENC_NA);
    offset+=string_len;

    /* Timestamp: YYMMDDhhmmssZ */
    /* They really came up with a two-digit year in late 1990s! =8o */
    timestr=tvb_get_ptr(tvb, offset, 13);
    proto_tree_add_string_format_value(tree, hf_pktc_timestamp, tvb, offset, 13, timestr,
                                "%.2s-%.2s-%.2s %.2s:%.2s:%.2s",
                                 timestr, timestr+2, timestr+4, timestr+6, timestr+8, timestr+10);
    offset+=13;

    /* app specific data */
    offset=dissect_pktc_app_specific_data(pinfo, tree, tvb, offset, doi, KMMID_REKEY);

    /* list of ciphersuites */
    offset=dissect_pktc_list_of_ciphersuites(pinfo, tree, tvb, offset, doi);

    /* sec param lifetime */
    proto_tree_add_item(tree, hf_pktc_sec_param_lifetime, tvb, offset, 4, ENC_BIG_ENDIAN);
    offset+=4;

    /* grace period */
    proto_tree_add_item(tree, hf_pktc_grace_period, tvb, offset, 4, ENC_BIG_ENDIAN);
    offset+=4;

    /* re-establish flag */
    proto_tree_add_item(tree, hf_pktc_reestablish_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset+=1;

    /* sha-1 hmac */
    proto_tree_add_item(tree, hf_pktc_sha1_hmac, tvb, offset, 20, ENC_NA);
    offset+=20;

    return offset;
}