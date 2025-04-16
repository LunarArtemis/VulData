void proto_register_c1222(void) {

  /* List of fields */
  static hf_register_info hf[] = {
   { &hf_c1222_epsem_flags,
    { "C12.22 EPSEM Flags", "c1222.epsem.flags",
    FT_UINT8, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_flags_reserved,
    { "C12.22 Reserved Flag", "c1222.epsem.flags.reserved",
    FT_BOOLEAN, 8,
    NULL, C1222_EPSEM_FLAG_RESERVED,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_flags_recovery,
    { "C12.22 Recovery Flag", "c1222.epsem.flags.recovery",
    FT_BOOLEAN, 8,
    NULL, C1222_EPSEM_FLAG_RECOVERY_SESSION,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_flags_proxy,
    { "C12.22 Proxy Service Used Flag", "c1222.epsem.flags.proxy",
    FT_BOOLEAN, 8,
    NULL, C1222_EPSEM_FLAG_PROXY_SERVICE_USED,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_flags_ed_class,
    { "C12.22 ED Class Flag", "c1222.epsem.flags.ed_class",
    FT_BOOLEAN, 8,
    NULL, C1222_EPSEM_FLAG_ED_CLASS_INCLUDED,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_flags_security_modes,
    { "C12.22 Security Mode Flags", "c1222.epsem.flags.security",
    FT_UINT8, BASE_HEX,
    VALS(c1222_security_modes), C1222_EPSEM_FLAG_SECURITY_MODE,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_flags_response_control,
    { "C12.22 Response Control Flags", "c1222.epsem.flags.response_control",
    FT_UINT8, BASE_HEX,
    VALS(c1222_response_control), C1222_EPSEM_FLAG_RESPONSE_CONTROL,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_ed_class,
    { "C12.22 EPSEM ED Class", "c1222.epsem.edclass",
    FT_BYTES, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_total,
    { "C12.22 EPSEM", "c1222.epsem.data",
    FT_BYTES, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_mac,
    { "C12.22 EPSEM MAC", "c1222.epsem.mac",
    FT_BYTES, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_cmd,
    { "C12.22 Command", "c1222.cmd",
    FT_UINT8, BASE_HEX,
    VALS(commandnames), 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_err,
    { "C12.22 Response", "c1222.err",
    FT_UINT8, BASE_HEX,
    VALS(commandnames), 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_logon_id,
    { "C12.22 Logon User-Id", "c1222.logon.id",
    FT_UINT16, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_logon_user,
    { "C12.22 Logon User", "c1222.logon.user",
    FT_STRING, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_security_password,
    { "C12.22 Security Password", "c1222.security.password",
    FT_STRING, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_auth_len,
    { "C12.22 Authenticate Request Length", "c1222.authenticate.len",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_auth_data,
    { "C12.22 Authenticate Data", "c1222.authenticate.data",
    FT_BYTES, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_read_table,
    { "C12.22 Table", "c1222.read.table",
    FT_UINT16, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_read_offset,
    { "C12.22 Offset", "c1222.read.offset",
    FT_UINT24, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_read_count,
    { "C12.22 Count", "c1222.read.count",
    FT_UINT16, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_write_table,
    { "C12.22 Table", "c1222.write.table",
    FT_UINT16, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_write_offset,
    { "C12.22 Offset", "c1222.write.offset",
    FT_UINT24, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_write_size,
    { "C12.22 Table Size", "c1222.write.size",
    FT_UINT16, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_write_data,
    { "C12.22 Table Data", "c1222.write.data",
    FT_BYTES, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_write_chksum,
    { "C12.22 Table Data Checksum", "c1222.write.chksum",
    FT_UINT8, BASE_HEX,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_write_chksum_status,
    { "C12.22 Table Data Checksum Status", "c1222.write.chksum.status",
    FT_UINT8, BASE_NONE,
    VALS(proto_checksum_vals), 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_procedure_response,
    { "C12.22 Procedure Response", "c1222.procedure.response",
    FT_UINT16, BASE_DEC,
    VALS(c1222_proc_response_control), C1222_PROCEDURE_RESPONSE,
    NULL, HFILL }
   },
   { &hf_c1222_procedure_mfg,
    { "C12.22 Procedure Mfg", "c1222.procedure.mfg",
    FT_UINT16, BASE_DEC,
    NULL, C1222_PROCEDURE_MFG,
    NULL, HFILL }
   },
   { &hf_c1222_procedure_num,
    { "C12.22 Procedure Number", "c1222.procedure.num",
    FT_UINT16, BASE_DEC,
    NULL, C1222_PROCEDURE_NUMBER,
    NULL, HFILL }
   },
   { &hf_c1222_procedure_sequence,
    { "C12.22 Procedure Sequence Number", "c1222.procedure.sequence",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_neg_pkt_size,
    { "C12.22 Negotiate Packet Size", "c1222.negotiate.pktsize",
    FT_UINT16, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_neg_nbr_pkts,
    { "C12.22 Negotiate Number of Packets", "c1222.negotiate.numpkts",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_wait_secs,
    { "C12.22 Wait Seconds", "c1222.wait.seconds",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_timing_setup_traffic,
    { "C12.22 Timing Setup Channel Traffic Timeout", "c1222.timingsetup.traffic",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_timing_setup_inter_char,
    { "C12.22 Timing Setup Intercharacter Timeout", "c1222.timingsetup.interchar",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_timing_setup_resp_to,
    { "C12.22 Timing Setup Response Timeout", "c1222.timingsetup.respto",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_timing_setup_nbr_retries,
    { "C12.22 Timing Setup Number of Retries", "c1222.timingsetup.nbrretries",
    FT_UINT8, BASE_DEC,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_data,
    { "C12.22 data", "c1222.data",
    FT_BYTES, BASE_NONE,
    NULL, 0x0,
    NULL, HFILL }
   },
   { &hf_c1222_epsem_crypto_good,
    { "Crypto good", "c1222.crypto_good",
    FT_BOOLEAN, BASE_NONE,
    NULL, 0x0,
    "True: crypto ok; False: doesn't match or not checked", HFILL }
   },
   { &hf_c1222_epsem_crypto_bad,
    { "Crypto bad", "c1222.crypto_bad",
    FT_BOOLEAN, BASE_NONE,
    NULL, 0x0,
    "True: crypto bad; False: crypto ok or not checked", HFILL }
   },

/*--- Included file: packet-c1222-hfarr.c ---*/
#line 1 "./asn1/c1222/packet-c1222-hfarr.c"
    { &hf_c1222_MESSAGE_PDU,
      { "MESSAGE", "c1222.MESSAGE_element",
        FT_NONE, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_aSO_context,
      { "aSO-context", "c1222.aSO_context",
        FT_OID, BASE_NONE, NULL, 0,
        "ASO_qualifier", HFILL }},
    { &hf_c1222_called_AP_title,
      { "called-AP-title", "c1222.called_AP_title",
        FT_UINT32, BASE_DEC, VALS(c1222_Called_AP_title_vals), 0,
        NULL, HFILL }},
    { &hf_c1222_called_AP_invocation_id,
      { "called-AP-invocation-id", "c1222.called_AP_invocation_id",
        FT_UINT32, BASE_DEC, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_calling_AP_title,
      { "calling-AP-title", "c1222.calling_AP_title",
        FT_UINT32, BASE_DEC, VALS(c1222_Calling_AP_title_vals), 0,
        NULL, HFILL }},
    { &hf_c1222_calling_AE_qualifier,
      { "calling-AE-qualifier", "c1222.calling_AE_qualifier",
        FT_UINT32, BASE_DEC, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_calling_AP_invocation_id,
      { "calling-AP-invocation-id", "c1222.calling_AP_invocation_id",
        FT_UINT32, BASE_DEC, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_mechanism_name,
      { "mechanism-name", "c1222.mechanism_name",
        FT_OID, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_calling_authentication_value,
      { "calling-authentication-value", "c1222.calling_authentication_value_element",
        FT_NONE, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_user_information,
      { "user-information", "c1222.user_information_element",
        FT_NONE, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_called_ap_title_abs,
      { "called-ap-title-abs", "c1222.called_ap_title_abs",
        FT_OID, BASE_NONE, NULL, 0,
        "OBJECT_IDENTIFIER", HFILL }},
    { &hf_c1222_called_ap_title_rel,
      { "called-ap-title-rel", "c1222.called_ap_title_rel",
        FT_REL_OID, BASE_NONE, NULL, 0,
        "RELATIVE_OID", HFILL }},
    { &hf_c1222_calling_ap_title_abs,
      { "calling-ap-title-abs", "c1222.calling_ap_title_abs",
        FT_OID, BASE_NONE, NULL, 0,
        "OBJECT_IDENTIFIER", HFILL }},
    { &hf_c1222_calling_ap_title_rel,
      { "calling-ap-title-rel", "c1222.calling_ap_title_rel",
        FT_REL_OID, BASE_NONE, NULL, 0,
        "RELATIVE_OID", HFILL }},
    { &hf_c1222_calling_authentication_value_indirect,
      { "calling-authentication-value-indirect", "c1222.calling_authentication_value_indirect",
        FT_INT32, BASE_DEC, NULL, 0,
        "INTEGER", HFILL }},
    { &hf_c1222_calling_authentication_value_encoding,
      { "calling-authentication-value-encoding", "c1222.calling_authentication_value_encoding",
        FT_UINT32, BASE_DEC, VALS(c1222_Authentication_value_encoding_vals), 0,
        "Authentication_value_encoding", HFILL }},
    { &hf_c1222_calling_authentication_value_single_asn1,
      { "calling-authentication-value-single-asn1", "c1222.calling_authentication_value_single_asn1",
        FT_UINT32, BASE_DEC, VALS(c1222_Calling_authentication_value_single_asn1_vals), 0,
        NULL, HFILL }},
    { &hf_c1222_calling_authentication_value_octet_aligned,
      { "calling-authentication-value-octet-aligned", "c1222.calling_authentication_value_octet_aligned",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING", HFILL }},
    { &hf_c1222_calling_authentication_value_c1222,
      { "calling-authentication-value-c1222", "c1222.calling_authentication_value_c1222_element",
        FT_NONE, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_calling_authentication_value_c1221,
      { "calling-authentication-value-c1221", "c1222.calling_authentication_value_c1221",
        FT_UINT32, BASE_DEC, VALS(c1222_Calling_authentication_value_c1221_U_vals), 0,
        NULL, HFILL }},
    { &hf_c1222_key_id_element,
      { "key-id-element", "c1222.key_id_element",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_iv_element,
      { "iv-element", "c1222.iv_element",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_c1222_c1221_auth_identification,
      { "c1221-auth-identification", "c1222.c1221_auth_identification",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING_SIZE_CONSTR001", HFILL }},
    { &hf_c1222_c1221_auth_request,
      { "c1221-auth-request", "c1222.c1221_auth_request",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING_SIZE_1_255", HFILL }},
    { &hf_c1222_c1221_auth_response,
      { "c1221-auth-response", "c1222.c1221_auth_response",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING_SIZE_CONSTR002", HFILL }},

/*--- End of included file: packet-c1222-hfarr.c ---*/
#line 1354 "./asn1/c1222/packet-c1222-template.c"
  };

  /* List of subtrees */
  static gint *ett[] = {
                  &ett_c1222,
                  &ett_c1222_epsem,
                  &ett_c1222_flags,
                  &ett_c1222_crypto,
                  &ett_c1222_cmd,

/*--- Included file: packet-c1222-ettarr.c ---*/
#line 1 "./asn1/c1222/packet-c1222-ettarr.c"
    &ett_c1222_MESSAGE_U,
    &ett_c1222_Called_AP_title,
    &ett_c1222_Calling_AP_title,
    &ett_c1222_Calling_authentication_value_U,
    &ett_c1222_Authentication_value_encoding,
    &ett_c1222_Calling_authentication_value_single_asn1,
    &ett_c1222_Calling_authentication_value_c1222_U,
    &ett_c1222_Calling_authentication_value_c1221_U,

/*--- End of included file: packet-c1222-ettarr.c ---*/
#line 1364 "./asn1/c1222/packet-c1222-template.c"
  };

  static ei_register_info ei[] = {
    { &ei_c1222_command_truncated, { "c1222.command_truncated", PI_MALFORMED, PI_ERROR, "C12.22 command truncated", EXPFILL }},
    { &ei_c1222_bad_checksum, { "c1222.bad_checksum", PI_CHECKSUM, PI_ERROR, "Bad checksum", EXPFILL }},
    { &ei_c1222_epsem_missing, { "c1222.epsem.missing", PI_MALFORMED, PI_ERROR, "C12.22 EPSEM missing", EXPFILL }},
    { &ei_c1222_epsem_failed_authentication, { "c1222.epsem.failed_authentication", PI_SECURITY, PI_ERROR, "C12.22 EPSEM failed authentication", EXPFILL }},
    { &ei_c1222_epsem_not_decryped, { "c1222.epsem.not_decryped", PI_UNDECODED, PI_WARN, "C12.22 EPSEM could not be decrypted", EXPFILL }},
    { &ei_c1222_ed_class_missing, { "c1222.ed_class_missing", PI_SECURITY, PI_ERROR, "C12.22 ED Class missing", EXPFILL }},
    { &ei_c1222_epsem_ber_length_error, { "c1222.epsem.ber_length_error", PI_MALFORMED, PI_ERROR, "C12.22 EPSEM BER length error", EXPFILL }},
    { &ei_c1222_epsem_field_length_error, { "c1222.epsem.field_length_error", PI_MALFORMED, PI_ERROR, "C12.22 EPSEM field length error", EXPFILL }},
    { &ei_c1222_mac_missing, { "c1222.mac_missing", PI_MALFORMED, PI_ERROR, "C12.22 MAC missing", EXPFILL }},
  };

  expert_module_t* expert_c1222;
  module_t *c1222_module;

  static uat_field_t c1222_uat_flds[] = {
    UAT_FLD_HEX(c1222_users,keynum,"Key ID","Key identifier in hexadecimal"),
    UAT_FLD_BUFFER(c1222_users, key, "Key", "Encryption key as 16-byte hex string"),
    UAT_END_FIELDS
  };

  /* Register protocol */
  proto_c1222 = proto_register_protocol(PNAME, PSNAME, PFNAME);
  /* Register fields and subtrees */
  proto_register_field_array(proto_c1222, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
  expert_c1222 = expert_register_protocol(proto_c1222);
  expert_register_field_array(expert_c1222, ei, array_length(ei));
  c1222_module = prefs_register_protocol(proto_c1222, proto_reg_handoff_c1222);
  prefs_register_bool_preference(c1222_module, "desegment",
        "Reassemble all C12.22 messages spanning multiple TCP segments",
        "Whether the C12.22 dissector should reassemble all messages spanning multiple TCP segments",
        &c1222_desegment);
  prefs_register_string_preference(c1222_module, "baseoid", "Base OID to use for relative OIDs",
        "Base object identifier for use in resolving relative object identifiers",
        &c1222_baseoid_str);
  prefs_register_bool_preference(c1222_module, "decrypt",
        "Verify crypto for all applicable C12.22 messages",
        "Whether the C12.22 dissector should verify the crypto for all relevant messages",
        &c1222_decrypt);
  prefs_register_bool_preference(c1222_module, "big_endian",
        "Interpret multibyte numbers as big endian",
        "Whether the C12.22 dissector should interpret procedure numbers as big-endian",
        &c1222_big_endian);

  c1222_uat = uat_new("Decryption Table",
      sizeof(c1222_uat_data_t),         /* record size */
      "c1222_decryption_table",         /* filename */
      TRUE,                             /* from_profile */
      &c1222_uat_data,                  /* data_ptr */
      &num_c1222_uat_data,              /* numitems_ptr */
      UAT_AFFECTS_DISSECTION,           /* affects dissection of packets, but not set of named fields */
      NULL,                             /* help */
      c1222_uat_data_copy_cb,           /* copy callback */
      c1222_uat_data_update_cb,         /* update callback */
      c1222_uat_data_free_cb,           /* free callback */
      NULL,                             /* post update callback */
      NULL,                             /* reset callback */
      c1222_uat_flds);                  /* UAT field definitions */

  prefs_register_uat_preference(c1222_module,
      "decryption_table",
      "Decryption Table",
      "Table of security parameters for decryption of C12.22 packets",
      c1222_uat);
}

/*--- proto_reg_handoff_c1222 ---------------------------------------*/
void
proto_reg_handoff_c1222(void)
{
  static gboolean initialized = FALSE;
  guint8 *temp = NULL;

  if( !initialized ) {
    c1222_handle = create_dissector_handle(dissect_c1222, proto_c1222);
    c1222_udp_handle = create_dissector_handle(dissect_c1222_common, proto_c1222);
    dissector_add_uint_with_preference("tcp.port", C1222_PORT, c1222_handle);
    dissector_add_uint_with_preference("udp.port", C1222_PORT, c1222_udp_handle);
    initialized = TRUE;
  }
  if (c1222_baseoid_str && (c1222_baseoid_str[0] != '\0') &&
      ((c1222_baseoid_len = oid_string2encoded(NULL, c1222_baseoid_str, &temp)) != 0)) {
    c1222_baseoid = (guint8 *)wmem_realloc(wmem_epan_scope(), c1222_baseoid, c1222_baseoid_len);
    memcpy(c1222_baseoid, temp, c1222_baseoid_len);
    wmem_free(NULL, temp);
  } else if (c1222_baseoid) {
      wmem_free(wmem_epan_scope(), c1222_baseoid);
      c1222_baseoid = NULL;
      c1222_baseoid_len = 0;
  }
}