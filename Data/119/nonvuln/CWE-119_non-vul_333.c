void tcm_loop_drop_scsi_hba(
	struct se_wwn *wwn)
{
	struct tcm_loop_hba *tl_hba = container_of(wwn,
				struct tcm_loop_hba, tl_hba_wwn);
	int host_no = tl_hba->sh->host_no;
	/*
	 * Call device_unregister() on the original tl_hba->dev.
	 * tcm_loop_fabric_scsi.c:tcm_loop_release_adapter() will
	 * release *tl_hba;
	 */
	device_unregister(&tl_hba->dev);

	printk(KERN_INFO "TCM_Loop_ConfigFS: Deallocated emulated Target"
		" SAS Address: %s at Linux/SCSI Host ID: %d\n",
		config_item_name(&wwn->wwn_group.cg_item), host_no);
}