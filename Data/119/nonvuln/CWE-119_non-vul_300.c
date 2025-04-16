static int tcm_loop_setup_hba_bus(struct tcm_loop_hba *tl_hba, int tcm_loop_host_id)
{
	int ret;

	tl_hba->dev.bus = &tcm_loop_lld_bus;
	tl_hba->dev.parent = tcm_loop_primary;
	tl_hba->dev.release = &tcm_loop_release_adapter;
	dev_set_name(&tl_hba->dev, "tcm_loop_adapter_%d", tcm_loop_host_id);

	ret = device_register(&tl_hba->dev);
	if (ret) {
		printk(KERN_ERR "device_register() failed for"
				" tl_hba->dev: %d\n", ret);
		return -ENODEV;
	}

	return 0;
}