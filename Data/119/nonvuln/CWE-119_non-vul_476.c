static void
nvmet_fc_tgt_a_put(struct nvmet_fc_tgt_assoc *assoc)
{
	kref_put(&assoc->ref, nvmet_fc_target_assoc_free);
}