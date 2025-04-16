static void tcm_loop_port_unlink(
	struct se_portal_group *se_tpg,
	struct se_lun *se_lun)
{
	struct scsi_device *sd;
	struct tcm_loop_hba *tl_hba;
	struct tcm_loop_tpg *tl_tpg;

	tl_tpg = container_of(se_tpg, struct tcm_loop_tpg, tl_se_tpg);
	tl_hba = tl_tpg->tl_hba;

	sd = scsi_device_lookup(tl_hba->sh, 0, tl_tpg->tl_tpgt,
				se_lun->unpacked_lun);
	if (!sd) {
		printk(KERN_ERR "Unable to locate struct scsi_device for %d:%d:"
			"%d\n", 0, tl_tpg->tl_tpgt, se_lun->unpacked_lun);
		return;
	}
	/*
	 * Remove Linux/SCSI struct scsi_device by HCTL
	 */
	scsi_remove_device(sd);
	scsi_device_put(sd);

	atomic_dec(&tl_tpg->tl_tpg_port_count);
	smp_mb__after_atomic_dec();

	printk(KERN_INFO "TCM_Loop_ConfigFS: Port Unlink Successful\n");
}