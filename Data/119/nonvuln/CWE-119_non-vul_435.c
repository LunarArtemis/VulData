static void
nvmet_fc_target_assoc_free(struct kref *ref)
{
	struct nvmet_fc_tgt_assoc *assoc =
		container_of(ref, struct nvmet_fc_tgt_assoc, ref);
	struct nvmet_fc_tgtport *tgtport = assoc->tgtport;
	unsigned long flags;

	spin_lock_irqsave(&tgtport->lock, flags);
	list_del(&assoc->a_list);
	spin_unlock_irqrestore(&tgtport->lock, flags);
	ida_simple_remove(&tgtport->assoc_cnt, assoc->a_id);
	kfree(assoc);
	nvmet_fc_tgtport_put(tgtport);
}