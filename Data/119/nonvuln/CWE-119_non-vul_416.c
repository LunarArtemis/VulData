static void
__nvmet_fc_free_assocs(struct nvmet_fc_tgtport *tgtport)
{
	struct nvmet_fc_tgt_assoc *assoc, *next;
	unsigned long flags;

	spin_lock_irqsave(&tgtport->lock, flags);
	list_for_each_entry_safe(assoc, next,
				&tgtport->assoc_list, a_list) {
		if (!nvmet_fc_tgt_a_get(assoc))
			continue;
		spin_unlock_irqrestore(&tgtport->lock, flags);
		nvmet_fc_delete_target_assoc(assoc);
		nvmet_fc_tgt_a_put(assoc);
		spin_lock_irqsave(&tgtport->lock, flags);
	}
	spin_unlock_irqrestore(&tgtport->lock, flags);
}