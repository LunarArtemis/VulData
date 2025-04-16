static int __init tcm_loop_fabric_init(void)
{
	int ret;

	tcm_loop_cmd_cache = kmem_cache_create("tcm_loop_cmd_cache",
				sizeof(struct tcm_loop_cmd),
				__alignof__(struct tcm_loop_cmd),
				0, NULL);
	if (!tcm_loop_cmd_cache) {
		printk(KERN_ERR "kmem_cache_create() for"
			" tcm_loop_cmd_cache failed\n");
		return -ENOMEM;
	}

	ret = tcm_loop_alloc_core_bus();
	if (ret)
		return ret;

	ret = tcm_loop_register_configfs();
	if (ret) {
		tcm_loop_release_core_bus();
		return ret;
	}

	return 0;
}