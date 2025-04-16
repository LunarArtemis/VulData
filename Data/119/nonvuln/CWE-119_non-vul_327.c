static int tcm_loop_register_configfs(void)
{
	struct target_fabric_configfs *fabric;
	struct config_group *tf_cg;
	int ret;
	/*
	 * Set the TCM Loop HBA counter to zero
	 */
	tcm_loop_hba_no_cnt = 0;
	/*
	 * Register the top level struct config_item_type with TCM core
	 */
	fabric = target_fabric_configfs_init(THIS_MODULE, "loopback");
	if (IS_ERR(fabric)) {
		printk(KERN_ERR "tcm_loop_register_configfs() failed!\n");
		return PTR_ERR(fabric);
	}
	/*
	 * Setup the fabric API of function pointers used by target_core_mod
	 */
	fabric->tf_ops.get_fabric_name = &tcm_loop_get_fabric_name;
	fabric->tf_ops.get_fabric_proto_ident = &tcm_loop_get_fabric_proto_ident;
	fabric->tf_ops.tpg_get_wwn = &tcm_loop_get_endpoint_wwn;
	fabric->tf_ops.tpg_get_tag = &tcm_loop_get_tag;
	fabric->tf_ops.tpg_get_default_depth = &tcm_loop_get_default_depth;
	fabric->tf_ops.tpg_get_pr_transport_id = &tcm_loop_get_pr_transport_id;
	fabric->tf_ops.tpg_get_pr_transport_id_len =
					&tcm_loop_get_pr_transport_id_len;
	fabric->tf_ops.tpg_parse_pr_out_transport_id =
					&tcm_loop_parse_pr_out_transport_id;
	fabric->tf_ops.tpg_check_demo_mode = &tcm_loop_check_demo_mode;
	fabric->tf_ops.tpg_check_demo_mode_cache =
					&tcm_loop_check_demo_mode_cache;
	fabric->tf_ops.tpg_check_demo_mode_write_protect =
					&tcm_loop_check_demo_mode_write_protect;
	fabric->tf_ops.tpg_check_prod_mode_write_protect =
					&tcm_loop_check_prod_mode_write_protect;
	/*
	 * The TCM loopback fabric module runs in demo-mode to a local
	 * virtual SCSI device, so fabric dependent initator ACLs are
	 * not required.
	 */
	fabric->tf_ops.tpg_alloc_fabric_acl = &tcm_loop_tpg_alloc_fabric_acl;
	fabric->tf_ops.tpg_release_fabric_acl =
					&tcm_loop_tpg_release_fabric_acl;
	fabric->tf_ops.tpg_get_inst_index = &tcm_loop_get_inst_index;
	/*
	 * Used for setting up remaining TCM resources in process context
	 */
	fabric->tf_ops.new_cmd_map = &tcm_loop_new_cmd_map;
	fabric->tf_ops.check_stop_free = &tcm_loop_check_stop_free;
	fabric->tf_ops.release_cmd = &tcm_loop_release_cmd;
	fabric->tf_ops.shutdown_session = &tcm_loop_shutdown_session;
	fabric->tf_ops.close_session = &tcm_loop_close_session;
	fabric->tf_ops.stop_session = &tcm_loop_stop_session;
	fabric->tf_ops.fall_back_to_erl0 = &tcm_loop_fall_back_to_erl0;
	fabric->tf_ops.sess_logged_in = &tcm_loop_sess_logged_in;
	fabric->tf_ops.sess_get_index = &tcm_loop_sess_get_index;
	fabric->tf_ops.sess_get_initiator_sid = NULL;
	fabric->tf_ops.write_pending = &tcm_loop_write_pending;
	fabric->tf_ops.write_pending_status = &tcm_loop_write_pending_status;
	/*
	 * Not used for TCM loopback
	 */
	fabric->tf_ops.set_default_node_attributes =
					&tcm_loop_set_default_node_attributes;
	fabric->tf_ops.get_task_tag = &tcm_loop_get_task_tag;
	fabric->tf_ops.get_cmd_state = &tcm_loop_get_cmd_state;
	fabric->tf_ops.queue_data_in = &tcm_loop_queue_data_in;
	fabric->tf_ops.queue_status = &tcm_loop_queue_status;
	fabric->tf_ops.queue_tm_rsp = &tcm_loop_queue_tm_rsp;
	fabric->tf_ops.set_fabric_sense_len = &tcm_loop_set_fabric_sense_len;
	fabric->tf_ops.get_fabric_sense_len = &tcm_loop_get_fabric_sense_len;
	fabric->tf_ops.is_state_remove = &tcm_loop_is_state_remove;

	tf_cg = &fabric->tf_group;
	/*
	 * Setup function pointers for generic logic in target_core_fabric_configfs.c
	 */
	fabric->tf_ops.fabric_make_wwn = &tcm_loop_make_scsi_hba;
	fabric->tf_ops.fabric_drop_wwn = &tcm_loop_drop_scsi_hba;
	fabric->tf_ops.fabric_make_tpg = &tcm_loop_make_naa_tpg;
	fabric->tf_ops.fabric_drop_tpg = &tcm_loop_drop_naa_tpg;
	/*
	 * fabric_post_link() and fabric_pre_unlink() are used for
	 * registration and release of TCM Loop Virtual SCSI LUNs.
	 */
	fabric->tf_ops.fabric_post_link = &tcm_loop_port_link;
	fabric->tf_ops.fabric_pre_unlink = &tcm_loop_port_unlink;
	fabric->tf_ops.fabric_make_np = NULL;
	fabric->tf_ops.fabric_drop_np = NULL;
	/*
	 * Setup default attribute lists for various fabric->tf_cit_tmpl
	 */
	TF_CIT_TMPL(fabric)->tfc_wwn_cit.ct_attrs = tcm_loop_wwn_attrs;
	TF_CIT_TMPL(fabric)->tfc_tpg_base_cit.ct_attrs = tcm_loop_tpg_attrs;
	TF_CIT_TMPL(fabric)->tfc_tpg_attrib_cit.ct_attrs = NULL;
	TF_CIT_TMPL(fabric)->tfc_tpg_param_cit.ct_attrs = NULL;
	TF_CIT_TMPL(fabric)->tfc_tpg_np_base_cit.ct_attrs = NULL;
	/*
	 * Once fabric->tf_ops has been setup, now register the fabric for
	 * use within TCM
	 */
	ret = target_fabric_configfs_register(fabric);
	if (ret < 0) {
		printk(KERN_ERR "target_fabric_configfs_register() for"
				" TCM_Loop failed!\n");
		target_fabric_configfs_free(fabric);
		return -1;
	}
	/*
	 * Setup our local pointer to *fabric.
	 */
	tcm_loop_fabric_configfs = fabric;
	printk(KERN_INFO "TCM_LOOP[0] - Set fabric ->"
			" tcm_loop_fabric_configfs\n");
	return 0;
}