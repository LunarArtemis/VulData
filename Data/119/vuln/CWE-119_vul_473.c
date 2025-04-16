static int
dissect_pktc_mtafqdn(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    int offset=0;
    proto_tree *pktc_mtafqdn_tree;
    proto_item *item;
    tvbuff_t *pktc_mtafqdn_tvb;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "PKTC");

    item = proto_tree_add_item(tree, proto_pktc, tvb, 0, 0, ENC_NA);
    pktc_mtafqdn_tree = proto_item_add_subtree(item, ett_pktc_mtafqdn);

    col_add_fstr(pinfo->cinfo, COL_INFO, "MTA FQDN %s",
                    pinfo->srcport == pinfo->match_uint ? "Reply":"Request");

    /* KRB_AP_RE[QP] */
    pktc_mtafqdn_tvb = tvb_new_subset_remaining(tvb, offset);
    offset += dissect_kerberos_main(pktc_mtafqdn_tvb, pinfo, pktc_mtafqdn_tree, FALSE, NULL);

    /* KRB_SAFE */
    pktc_mtafqdn_tvb = tvb_new_subset_remaining(tvb, offset);
    offset += dissect_kerberos_main(pktc_mtafqdn_tvb, pinfo, pktc_mtafqdn_tree, FALSE, cb);

    proto_item_set_len(item, offset);
    return tvb_captured_length(tvb);
}