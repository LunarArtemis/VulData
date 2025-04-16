void
proto_register_bthci_iso(void)
{
    module_t         *bthci_iso_module;
    expert_module_t  *bthci_iso_expert_module;
    static hf_register_info hf[] = {
        { &hf_bthci_iso_chandle,
          { "Connection Handle",           "bthci_iso.chandle",
            FT_UINT16, BASE_HEX, NULL, 0x0FFF,
            NULL, HFILL }
        },
        { &hf_bthci_iso_pb_flag,
          { "PB Flag",               "bthci_iso.pb_flag",
            FT_UINT16, BASE_HEX, VALS(iso_pb_flag_vals), 0x3000,
            "Packet Boundary Flag", HFILL }
        },
        { &hf_bthci_iso_ts_flag,
          { "Timestamp present",               "bthci_iso.ts_flag",
            FT_BOOLEAN, 16, NULL, 0x4000,
            NULL, HFILL }
        },
        { &hf_bthci_iso_reserved,
          { "Reserved",                    "bthci_iso.reserved",
            FT_UINT16, BASE_HEX, NULL, 0x8000,
            NULL, HFILL }
        },
        { &hf_bthci_iso_continuation_to,
          { "This is a continuation to the PDU in frame",    "bthci_iso.continuation_to",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "This is a continuation to the PDU in frame #", HFILL }
        },
        { &hf_bthci_iso_reassembled_in,
          { "This PDU is reassembled in frame",              "bthci_iso.reassembled_in",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "This PDU is reassembled in frame #", HFILL }
        },
        { &hf_bthci_iso_connect_in,
          { "Connect in frame",            "bthci_iso.connect_in",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_bthci_iso_disconnect_in,
          { "Disconnect in frame",         "bthci_iso.disconnect_in",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_bthci_iso_data_length,
          { "Data Length",           "bthci_iso.data_length",
            FT_UINT16, BASE_DEC, NULL, 0x3FFF,
            NULL, HFILL }
        },
        { &hf_bthci_iso_data,
          { "Data",                        "bthci_iso.data",
            FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_bthci_iso,
    };

    static ei_register_info ei[] = {
        { &ei_length_bad,      { "bthci_iso.length.bad",      PI_MALFORMED, PI_WARN, "Length too short", EXPFILL }},
    };

    /* Register the protocol name and description */
    proto_bthci_iso = proto_register_protocol("Bluetooth HCI ISO Packet", "HCI_ISO", "bthci_iso");
    bthci_iso_handle = register_dissector("bthci_iso", dissect_bthci_iso, proto_bthci_iso);

    /* Required function calls to register the header fields and subtrees used */
    proto_register_field_array(proto_bthci_iso, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    bthci_iso_expert_module = expert_register_protocol(proto_bthci_iso);
    expert_register_field_array(bthci_iso_expert_module, ei, array_length(ei));

    /* Register configuration preferences */
    bthci_iso_module = prefs_register_protocol_subtree("Bluetooth", proto_bthci_iso, NULL);
    prefs_register_bool_preference(bthci_iso_module, "hci_iso_reassembly",
        "Reassemble ISO Fragments",
        "Whether the ISO dissector should reassemble fragmented PDUs",
        &iso_reassembly);

    chandle_tree = wmem_tree_new_autoreset(wmem_epan_scope(), wmem_file_scope());
}