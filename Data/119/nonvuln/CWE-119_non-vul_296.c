static void tcm_loop_release_adapter(struct device *dev)
{
	struct tcm_loop_hba *tl_hba = to_tcm_loop_hba(dev);

	kfree(tl_hba);
}