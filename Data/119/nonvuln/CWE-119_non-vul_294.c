struct se_wwn *tcm_loop_make_scsi_hba(
	struct target_fabric_configfs *tf,
	struct config_group *group,
	const char *name)
{
	struct tcm_loop_hba *tl_hba;
	struct Scsi_Host *sh;
	char *ptr;
	int ret, off = 0;

	tl_hba = kzalloc(sizeof(struct tcm_loop_hba), GFP_KERNEL);
	if (!tl_hba) {
		printk(KERN_ERR "Unable to allocate struct tcm_loop_hba\n");
		return ERR_PTR(-ENOMEM);
	}
	/*
	 * Determine the emulated Protocol Identifier and Target Port Name
	 * based on the incoming configfs directory name.
	 */
	ptr = strstr(name, "naa.");
	if (ptr) {
		tl_hba->tl_proto_id = SCSI_PROTOCOL_SAS;
		goto check_len;
	}
	ptr = strstr(name, "fc.");
	if (ptr) {
		tl_hba->tl_proto_id = SCSI_PROTOCOL_FCP;
		off = 3; /* Skip over "fc." */
		goto check_len;
	}
	ptr = strstr(name, "iqn.");
	if (ptr) {
		tl_hba->tl_proto_id = SCSI_PROTOCOL_ISCSI;
		goto check_len;
	}

	printk(KERN_ERR "Unable to locate prefix for emulated Target Port:"
			" %s\n", name);
	return ERR_PTR(-EINVAL);

check_len:
	if (strlen(name) >= TL_WWN_ADDR_LEN) {
		printk(KERN_ERR "Emulated NAA %s Address: %s, exceeds"
			" max: %d\n", name, tcm_loop_dump_proto_id(tl_hba),
			TL_WWN_ADDR_LEN);
		kfree(tl_hba);
		return ERR_PTR(-EINVAL);
	}
	snprintf(&tl_hba->tl_wwn_address[0], TL_WWN_ADDR_LEN, "%s", &name[off]);

	/*
	 * Call device_register(tl_hba->dev) to register the emulated
	 * Linux/SCSI LLD of type struct Scsi_Host at tl_hba->sh after
	 * device_register() callbacks in tcm_loop_driver_probe()
	 */
	ret = tcm_loop_setup_hba_bus(tl_hba, tcm_loop_hba_no_cnt);
	if (ret)
		goto out;

	sh = tl_hba->sh;
	tcm_loop_hba_no_cnt++;
	printk(KERN_INFO "TCM_Loop_ConfigFS: Allocated emulated Target"
		" %s Address: %s at Linux/SCSI Host ID: %d\n",
		tcm_loop_dump_proto_id(tl_hba), name, sh->host_no);

	return &tl_hba->tl_hba_wwn;
out:
	kfree(tl_hba);
	return ERR_PTR(ret);
}