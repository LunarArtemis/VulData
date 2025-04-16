static int
nvmet_fc_tgt_a_get(struct nvmet_fc_tgt_assoc *assoc)
{
	return kref_get_unless_zero(&assoc->ref);
}