void
proto_register_pktc_mtafqdn(void)
{
    static hf_register_info hf[] = {
       { &hf_pktc_mtafqdn_msgtype, {
           "Message Type", "pktc.mtafqdn.msgtype", FT_UINT8, BASE_DEC,
           VALS(pktc_mtafqdn_msgtype_vals), 0, "MTA FQDN Message Type", HFILL }},
       { &hf_pktc_mtafqdn_enterprise, {
           "Enterprise Number", "pktc.mtafqdn.enterprise", FT_UINT32, BASE_DEC,
           NULL, 0, NULL, HFILL }},
       { &hf_pktc_mtafqdn_version, {
           "Protocol Version", "pktc.mtafqdn.version", FT_UINT8, BASE_DEC,
           NULL, 0, "MTA FQDN Protocol Version", HFILL }},
       /* MTA FQDN REQ */
       { &hf_pktc_mtafqdn_mac, {
           "MTA MAC address", "pktc.mtafqdn.mac", FT_ETHER, BASE_NONE,
           NULL, 0, NULL, HFILL }},
       { &hf_pktc_mtafqdn_pub_key_hash, {
           "MTA Public Key Hash", "pktc.mtafqdn.pub_key_hash", FT_BYTES, BASE_NONE,
           NULL, 0, "MTA Public Key Hash (SHA-1)", HFILL }},
       { &hf_pktc_mtafqdn_manu_cert_revoked, {
           "Manufacturer Cert Revocation Time", "pktc.mtafqdn.manu_cert_revoked", FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL,
           NULL, 0, "Manufacturer Cert Revocation Time (UTC) or 0 if not revoked", HFILL }},
       /* MTA FQDN REP */
       { &hf_pktc_mtafqdn_fqdn, {
           "MTA FQDN", "pktc.mtafqdn.fqdn", FT_STRING, BASE_NONE,
           NULL, 0, NULL, HFILL }},
       { &hf_pktc_mtafqdn_ip, {
           "MTA IP Address", "pktc.mtafqdn.ip", FT_IPv4, BASE_NONE,
           NULL, 0, "MTA IP Address (all zeros if not supplied)", HFILL }},
    };
    static gint *ett[] = {
        &ett_pktc_mtafqdn,
    };

    static ei_register_info ei[] = {
        { &ei_pktc_unknown_kmmid, { "pktc.mtafqdn.unknown_kmmid", PI_PROTOCOL, PI_WARN, "Unknown KMMID", EXPFILL }},
        { &ei_pktc_unknown_doi, { "pktc.mtafqdn.unknown_doi", PI_PROTOCOL, PI_WARN, "Unknown DOI", EXPFILL }},
    };

    expert_module_t* expert_pktc;

    proto_pktc_mtafqdn = proto_register_protocol("PacketCable MTA FQDN", "PKTC MTA FQDN", "pktc.mtafqdn");

    proto_register_field_array(proto_pktc_mtafqdn, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
    expert_pktc = expert_register_protocol(proto_pktc_mtafqdn);
    expert_register_field_array(expert_pktc, ei, array_length(ei));
}