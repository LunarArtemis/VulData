static void tcm_loop_deregister_configfs(void)
{
	if (!tcm_loop_fabric_configfs)
		return;

	target_fabric_configfs_deregister(tcm_loop_fabric_configfs);
	tcm_loop_fabric_configfs = NULL;
	printk(KERN_INFO "TCM_LOOP[0] - Cleared"
				" tcm_loop_fabric_configfs\n");
}