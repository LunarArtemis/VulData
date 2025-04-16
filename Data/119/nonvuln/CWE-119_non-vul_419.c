static int __init nvmet_fc_init_module(void)
{
	return nvmet_register_transport(&nvmet_fc_tgt_fcp_ops);
}