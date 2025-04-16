static struct nvmet_fc_tgt_assoc *
nvmet_fc_find_target_assoc(struct nvmet_fc_tgtport *tgtport,
				u64 association_id)
{
	struct nvmet_fc_tgt_assoc *assoc;
	struct nvmet_fc_tgt_assoc *ret = NULL;
	unsigned long flags;

	spin_lock_irqsave(&tgtport->lock, flags);
	list_for_each_entry(assoc, &tgtport->assoc_list, a_list) {
		if (association_id == assoc->association_id) {
			ret = assoc;
			nvmet_fc_tgt_a_get(assoc);
			break;
		}
	}
	spin_unlock_irqrestore(&tgtport->lock, flags);

	return ret;
}