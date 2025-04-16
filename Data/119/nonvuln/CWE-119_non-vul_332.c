static int tcm_loop_make_nexus(
	struct tcm_loop_tpg *tl_tpg,
	const char *name)
{
	struct se_portal_group *se_tpg;
	struct tcm_loop_hba *tl_hba = tl_tpg->tl_hba;
	struct tcm_loop_nexus *tl_nexus;
	int ret = -ENOMEM;

	if (tl_tpg->tl_hba->tl_nexus) {
		printk(KERN_INFO "tl_tpg->tl_hba->tl_nexus already exists\n");
		return -EEXIST;
	}
	se_tpg = &tl_tpg->tl_se_tpg;

	tl_nexus = kzalloc(sizeof(struct tcm_loop_nexus), GFP_KERNEL);
	if (!tl_nexus) {
		printk(KERN_ERR "Unable to allocate struct tcm_loop_nexus\n");
		return -ENOMEM;
	}
	/*
	 * Initialize the struct se_session pointer
	 */
	tl_nexus->se_sess = transport_init_session();
	if (IS_ERR(tl_nexus->se_sess)) {
		ret = PTR_ERR(tl_nexus->se_sess);
		goto out;
	}
	/*
	 * Since we are running in 'demo mode' this call with generate a
	 * struct se_node_acl for the tcm_loop struct se_portal_group with the SCSI
	 * Initiator port name of the passed configfs group 'name'.
	 */
	tl_nexus->se_sess->se_node_acl = core_tpg_check_initiator_node_acl(
				se_tpg, (unsigned char *)name);
	if (!tl_nexus->se_sess->se_node_acl) {
		transport_free_session(tl_nexus->se_sess);
		goto out;
	}
	/*
	 * Now, register the SAS I_T Nexus as active with the call to
	 * transport_register_session()
	 */
	__transport_register_session(se_tpg, tl_nexus->se_sess->se_node_acl,
			tl_nexus->se_sess, tl_nexus);
	tl_tpg->tl_hba->tl_nexus = tl_nexus;
	printk(KERN_INFO "TCM_Loop_ConfigFS: Established I_T Nexus to emulated"
		" %s Initiator Port: %s\n", tcm_loop_dump_proto_id(tl_hba),
		name);
	return 0;

out:
	kfree(tl_nexus);
	return ret;
}