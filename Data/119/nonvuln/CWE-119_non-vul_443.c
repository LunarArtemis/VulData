static void __exit nvmet_fc_exit_module(void)
{
	/* sanity check - all lports should be removed */
	if (!list_empty(&nvmet_fc_target_list))
		pr_warn("%s: targetport list not empty\n", __func__);

	nvmet_unregister_transport(&nvmet_fc_tgt_fcp_ops);

	ida_destroy(&nvmet_fc_tgtport_cnt);
}