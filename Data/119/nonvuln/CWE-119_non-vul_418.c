static struct nvmet_fc_tgt_assoc *
nvmet_fc_alloc_target_assoc(struct nvmet_fc_tgtport *tgtport)
{
	struct nvmet_fc_tgt_assoc *assoc, *tmpassoc;
	unsigned long flags;
	u64 ran;
	int idx;
	bool needrandom = true;

	assoc = kzalloc(sizeof(*assoc), GFP_KERNEL);
	if (!assoc)
		return NULL;

	idx = ida_simple_get(&tgtport->assoc_cnt, 0, 0, GFP_KERNEL);
	if (idx < 0)
		goto out_free_assoc;

	if (!nvmet_fc_tgtport_get(tgtport))
		goto out_ida_put;

	assoc->tgtport = tgtport;
	assoc->a_id = idx;
	INIT_LIST_HEAD(&assoc->a_list);
	kref_init(&assoc->ref);

	while (needrandom) {
		get_random_bytes(&ran, sizeof(ran) - BYTES_FOR_QID);
		ran = ran << BYTES_FOR_QID_SHIFT;

		spin_lock_irqsave(&tgtport->lock, flags);
		needrandom = false;
		list_for_each_entry(tmpassoc, &tgtport->assoc_list, a_list)
			if (ran == tmpassoc->association_id) {
				needrandom = true;
				break;
			}
		if (!needrandom) {
			assoc->association_id = ran;
			list_add_tail(&assoc->a_list, &tgtport->assoc_list);
		}
		spin_unlock_irqrestore(&tgtport->lock, flags);
	}

	return assoc;

out_ida_put:
	ida_simple_remove(&tgtport->assoc_cnt, idx);
out_free_assoc:
	kfree(assoc);
	return NULL;
}