static ssize_t tcm_loop_tpg_store_nexus(
	struct se_portal_group *se_tpg,
	const char *page,
	size_t count)
{
	struct tcm_loop_tpg *tl_tpg = container_of(se_tpg,
			struct tcm_loop_tpg, tl_se_tpg);
	struct tcm_loop_hba *tl_hba = tl_tpg->tl_hba;
	unsigned char i_port[TL_WWN_ADDR_LEN], *ptr, *port_ptr;
	int ret;
	/*
	 * Shutdown the active I_T nexus if 'NULL' is passed..
	 */
	if (!strncmp(page, "NULL", 4)) {
		ret = tcm_loop_drop_nexus(tl_tpg);
		return (!ret) ? count : ret;
	}
	/*
	 * Otherwise make sure the passed virtual Initiator port WWN matches
	 * the fabric protocol_id set in tcm_loop_make_scsi_hba(), and call
	 * tcm_loop_make_nexus()
	 */
	if (strlen(page) >= TL_WWN_ADDR_LEN) {
		printk(KERN_ERR "Emulated NAA Sas Address: %s, exceeds"
				" max: %d\n", page, TL_WWN_ADDR_LEN);
		return -EINVAL;
	}
	snprintf(&i_port[0], TL_WWN_ADDR_LEN, "%s", page);

	ptr = strstr(i_port, "naa.");
	if (ptr) {
		if (tl_hba->tl_proto_id != SCSI_PROTOCOL_SAS) {
			printk(KERN_ERR "Passed SAS Initiator Port %s does not"
				" match target port protoid: %s\n", i_port,
				tcm_loop_dump_proto_id(tl_hba));
			return -EINVAL;
		}
		port_ptr = &i_port[0];
		goto check_newline;
	}
	ptr = strstr(i_port, "fc.");
	if (ptr) {
		if (tl_hba->tl_proto_id != SCSI_PROTOCOL_FCP) {
			printk(KERN_ERR "Passed FCP Initiator Port %s does not"
				" match target port protoid: %s\n", i_port,
				tcm_loop_dump_proto_id(tl_hba));
			return -EINVAL;
		}
		port_ptr = &i_port[3]; /* Skip over "fc." */
		goto check_newline;
	}
	ptr = strstr(i_port, "iqn.");
	if (ptr) {
		if (tl_hba->tl_proto_id != SCSI_PROTOCOL_ISCSI) {
			printk(KERN_ERR "Passed iSCSI Initiator Port %s does not"
				" match target port protoid: %s\n", i_port,
				tcm_loop_dump_proto_id(tl_hba));
			return -EINVAL;
		}
		port_ptr = &i_port[0];
		goto check_newline;
	}
	printk(KERN_ERR "Unable to locate prefix for emulated Initiator Port:"
			" %s\n", i_port);
	return -EINVAL;
	/*
	 * Clear any trailing newline for the NAA WWN
	 */
check_newline:
	if (i_port[strlen(i_port)-1] == '\n')
		i_port[strlen(i_port)-1] = '\0';

	ret = tcm_loop_make_nexus(tl_tpg, port_ptr);
	if (ret < 0)
		return ret;

	return count;
}