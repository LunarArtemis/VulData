static int tcm_loop_drop_nexus(
	struct tcm_loop_tpg *tpg)
{
	struct se_session *se_sess;
	struct tcm_loop_nexus *tl_nexus;
	struct tcm_loop_hba *tl_hba = tpg->tl_hba;

	tl_nexus = tpg->tl_hba->tl_nexus;
	if (!tl_nexus)
		return -ENODEV;

	se_sess = tl_nexus->se_sess;
	if (!se_sess)
		return -ENODEV;

	if (atomic_read(&tpg->tl_tpg_port_count)) {
		printk(KERN_ERR "Unable to remove TCM_Loop I_T Nexus with"
			" active TPG port count: %d\n",
			atomic_read(&tpg->tl_tpg_port_count));
		return -EPERM;
	}

	printk(KERN_INFO "TCM_Loop_ConfigFS: Removing I_T Nexus to emulated"
		" %s Initiator Port: %s\n", tcm_loop_dump_proto_id(tl_hba),
		tl_nexus->se_sess->se_node_acl->initiatorname);
	/*
	 * Release the SCSI I_T Nexus to the emulated SAS Target Port
	 */
	transport_deregister_session(tl_nexus->se_sess);
	tpg->tl_hba->tl_nexus = NULL;
	kfree(tl_nexus);
	return 0;
}