static void
nvmet_fc_delete_ctrl(struct nvmet_ctrl *ctrl)
{
	struct nvmet_fc_tgtport *tgtport, *next;
	struct nvmet_fc_tgt_assoc *assoc;
	struct nvmet_fc_tgt_queue *queue;
	unsigned long flags;
	bool found_ctrl = false;

	/* this is a bit ugly, but don't want to make locks layered */
	spin_lock_irqsave(&nvmet_fc_tgtlock, flags);
	list_for_each_entry_safe(tgtport, next, &nvmet_fc_target_list,
			tgt_list) {
		if (!nvmet_fc_tgtport_get(tgtport))
			continue;
		spin_unlock_irqrestore(&nvmet_fc_tgtlock, flags);

		spin_lock_irqsave(&tgtport->lock, flags);
		list_for_each_entry(assoc, &tgtport->assoc_list, a_list) {
			queue = assoc->queues[0];
			if (queue && queue->nvme_sq.ctrl == ctrl) {
				if (nvmet_fc_tgt_a_get(assoc))
					found_ctrl = true;
				break;
			}
		}
		spin_unlock_irqrestore(&tgtport->lock, flags);

		nvmet_fc_tgtport_put(tgtport);

		if (found_ctrl) {
			nvmet_fc_delete_target_assoc(assoc);
			nvmet_fc_tgt_a_put(assoc);
			return;
		}

		spin_lock_irqsave(&nvmet_fc_tgtlock, flags);
	}
	spin_unlock_irqrestore(&nvmet_fc_tgtlock, flags);
}