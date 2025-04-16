static ssize_t tcm_loop_wwn_show_attr_version(
	struct target_fabric_configfs *tf,
	char *page)
{
	return sprintf(page, "TCM Loopback Fabric module %s\n", TCM_LOOP_VERSION);
}