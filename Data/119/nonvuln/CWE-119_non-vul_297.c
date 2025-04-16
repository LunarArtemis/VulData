static int tcm_loop_driver_probe(struct device *dev)
{
	struct tcm_loop_hba *tl_hba;
	struct Scsi_Host *sh;
	int error;

	tl_hba = to_tcm_loop_hba(dev);

	sh = scsi_host_alloc(&tcm_loop_driver_template,
			sizeof(struct tcm_loop_hba));
	if (!sh) {
		printk(KERN_ERR "Unable to allocate struct scsi_host\n");
		return -ENODEV;
	}
	tl_hba->sh = sh;

	/*
	 * Assign the struct tcm_loop_hba pointer to struct Scsi_Host->hostdata
	 */
	*((struct tcm_loop_hba **)sh->hostdata) = tl_hba;
	/*
	 * Setup single ID, Channel and LUN for now..
	 */
	sh->max_id = 2;
	sh->max_lun = 0;
	sh->max_channel = 0;
	sh->max_cmd_len = TL_SCSI_MAX_CMD_LEN;

	error = scsi_add_host(sh, &tl_hba->dev);
	if (error) {
		printk(KERN_ERR "%s: scsi_add_host failed\n", __func__);
		scsi_host_put(sh);
		return -ENODEV;
	}
	return 0;
}