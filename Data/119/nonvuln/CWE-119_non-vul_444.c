int
nvmet_fc_unregister_targetport(struct nvmet_fc_target_port *target_port)
{
	struct nvmet_fc_tgtport *tgtport = targetport_to_tgtport(target_port);

	/* terminate any outstanding associations */
	__nvmet_fc_free_assocs(tgtport);

	nvmet_fc_tgtport_put(tgtport);

	return 0;
}