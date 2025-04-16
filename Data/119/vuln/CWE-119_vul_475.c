void
proto_register_pktc(void)
{
    static hf_register_info hf[] = {
        { &hf_pktc_kmmid, {
            "Key Management Message ID", "pktc.kmmid", FT_UINT8, BASE_HEX,
            VALS(kmmid_types), 0, NULL, HFILL }},
        { &hf_pktc_doi, {
            "Domain of Interpretation", "pktc.doi", FT_UINT8, BASE_DEC,
            VALS(doi_types), 0, NULL, HFILL }},
        { &hf_pktc_version_major, {
            "Major version", "pktc.version.major", FT_UINT8, BASE_DEC,
            NULL, 0xF0, "Major version of PKTC", HFILL }},
        { &hf_pktc_version_minor, {
            "Minor version", "pktc.version.minor", FT_UINT8, BASE_DEC,
            NULL, 0x0F, "Minor version of PKTC", HFILL }},
        { &hf_pktc_server_nonce, {
            "Server Nonce", "pktc.server_nonce", FT_UINT32, BASE_HEX,
            NULL, 0, "Server Nonce random number", HFILL }},
        { &hf_pktc_server_principal, {
            "Server Kerberos Principal Identifier", "pktc.server_principal", FT_STRING, BASE_NONE,
            NULL, 0, NULL, HFILL }},
        { &hf_pktc_timestamp, {
            "Timestamp", "pktc.timestamp", FT_STRING, BASE_NONE,
            NULL, 0, "Timestamp (UTC)", HFILL }},
        { &hf_pktc_app_spec_data, {
            "Application Specific Data", "pktc.asd", FT_NONE, BASE_NONE,
            NULL, 0, "KMMID/DOI application specific data", HFILL }},
        { &hf_pktc_list_of_ciphersuites, {
            "List of Ciphersuites", "pktc.ciphers", FT_NONE, BASE_NONE,
            NULL, 0, NULL, HFILL }},
        { &hf_pktc_list_of_ciphersuites_len, {
            "Number of Ciphersuites", "pktc.ciphers.len", FT_UINT8, BASE_DEC,
            NULL, 0, NULL, HFILL }},
        { &hf_pktc_snmpAuthenticationAlgorithm, {
           "SNMPv3 Authentication Algorithm", "pktc.asd.snmp_auth_alg", FT_UINT8, BASE_HEX,
           VALS(snmp_authentication_algorithm_vals), 0, NULL, HFILL }},
        { &hf_pktc_snmpEncryptionTransformID, {
           "SNMPv3 Encryption Transform ID", "pktc.asd.snmp_enc_alg", FT_UINT8, BASE_HEX,
           VALS(snmp_transform_id_vals), 0, NULL, HFILL }},
        { &hf_pktc_ipsecAuthenticationAlgorithm, {
           "IPsec Authentication Algorithm", "pktc.asd.ipsec_auth_alg", FT_UINT8, BASE_HEX,
           VALS(ipsec_authentication_algorithm_vals), 0, NULL, HFILL }},
        { &hf_pktc_ipsecEncryptionTransformID, {
           "IPsec Encryption Transform ID", "pktc.asd.ipsec_enc_alg", FT_UINT8, BASE_HEX,
           VALS(ipsec_transform_id_vals), 0, NULL, HFILL }},
        { &hf_pktc_snmpEngineID_len, {
           "SNMPv3 Engine ID Length", "pktc.asd.snmp_engine_id.len", FT_UINT8, BASE_DEC,
           NULL, 0, "Length of SNMPv3 Engine ID", HFILL }},
        { &hf_pktc_snmpEngineID, {
           "SNMPv3 Engine ID", "pktc.asd.snmp_engine_id", FT_BYTES, BASE_NONE,
           NULL, 0, NULL, HFILL }},
        { &hf_pktc_snmpEngineBoots, {
           "SNMPv3 Engine Boots", "pktc.asd.snmp_engine_boots", FT_UINT32, BASE_DEC,
           NULL, 0, NULL, HFILL }},
        { &hf_pktc_snmpEngineTime, {
           "SNMPv3 Engine Time", "pktc.asd.snmp_engine_time", FT_UINT32, BASE_DEC,
           NULL, 0, "SNMPv3 Engine ID Time", HFILL }},
        { &hf_pktc_usmUserName_len, {
           "SNMPv3 USM User Name Length", "pktc.asd.snmp_usm_username.len", FT_UINT8, BASE_DEC,
           NULL, 0, "Length of SNMPv3 USM User Name", HFILL }},
        { &hf_pktc_usmUserName, {
           "SNMPv3 USM User Name", "pktc.asd.snmp_usm_username", FT_STRING, BASE_NONE,
           NULL, 0, NULL, HFILL }},
        { &hf_pktc_ipsec_spi, {
           "IPsec Security Parameter Index", "pktc.asd.ipsec_spi", FT_UINT32, BASE_HEX,
           NULL, 0, "Security Parameter Index for inbound Security Association (IPsec)", HFILL }},
        { &hf_pktc_reestablish_flag, {
            "Re-establish Flag", "pktc.reestablish", FT_BOOLEAN, BASE_NONE,
            NULL, 0x0, NULL, HFILL }},
        { &hf_pktc_ack_required_flag, {
            "ACK Required Flag", "pktc.ack_required", FT_BOOLEAN, BASE_NONE,
            NULL, 0x0, NULL, HFILL }},
        { &hf_pktc_sec_param_lifetime, {
            "Security Parameter Lifetime", "pktc.spl", FT_UINT32, BASE_DEC,
            NULL, 0, "Lifetime in seconds of security parameter", HFILL }},
        { &hf_pktc_sha1_hmac, {
           "SHA-1 HMAC", "pktc.sha1_hmac", FT_BYTES, BASE_NONE,
           NULL, 0, NULL, HFILL }},
        { &hf_pktc_grace_period, {
            "Grace Period", "pktc.grace_period", FT_UINT32, BASE_DEC,
            NULL, 0, "Grace Period in seconds", HFILL }},
    };
    static gint *ett[] = {
        &ett_pktc,
        &ett_pktc_app_spec_data,
        &ett_pktc_list_of_ciphersuites,
        &ett_pktc_engineid,
        &ett_pktc_version,
    };

    proto_pktc = proto_register_protocol("PacketCable", "PKTC", "pktc");
    proto_register_field_array(proto_pktc, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
}