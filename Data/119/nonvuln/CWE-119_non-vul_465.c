static int
nvmet_fc_tgtport_get(struct nvmet_fc_tgtport *tgtport)
{
	return kref_get_unless_zero(&tgtport->ref);
}