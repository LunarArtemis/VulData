static void
nvmet_fc_tgtport_put(struct nvmet_fc_tgtport *tgtport)
{
	kref_put(&tgtport->ref, nvmet_fc_free_tgtport);
}