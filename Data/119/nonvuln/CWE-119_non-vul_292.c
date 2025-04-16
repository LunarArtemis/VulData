static int tcm_loop_port_link(
	struct se_portal_group *se_tpg,
	struct se_lun *lun)
{
	struct tcm_loop_tpg *tl_tpg = container_of(se_tpg,
				struct tcm_loop_tpg, tl_se_tpg);
	struct tcm_loop_hba *tl_hba = tl_tpg->tl_hba;

	atomic_inc(&tl_tpg->tl_tpg_port_count);
	smp_mb__after_atomic_inc();
	/*
	 * Add Linux/SCSI struct scsi_device by HCTL
	 */
	scsi_add_device(tl_hba->sh, 0, tl_tpg->tl_tpgt, lun->unpacked_lun);

	printk(KERN_INFO "TCM_Loop_ConfigFS: Port Link Successful\n");
	return 0;
}