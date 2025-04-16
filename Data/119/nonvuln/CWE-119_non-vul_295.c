static void __exit tcm_loop_fabric_exit(void)
{
	tcm_loop_deregister_configfs();
	tcm_loop_release_core_bus();
	kmem_cache_destroy(tcm_loop_cmd_cache);
}