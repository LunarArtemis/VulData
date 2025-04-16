static void
nvmet_fc_delete_target_assoc(struct nvmet_fc_tgt_assoc *assoc)
{
	struct nvmet_fc_tgtport *tgtport = assoc->tgtport;
	struct nvmet_fc_tgt_queue *queue;
	unsigned long flags;
	int i;

	spin_lock_irqsave(&tgtport->lock, flags);
	for (i = NVMET_NR_QUEUES; i >= 0; i--) {
		queue = assoc->queues[i];
		if (queue) {
			if (!nvmet_fc_tgt_q_get(queue))
				continue;
			spin_unlock_irqrestore(&tgtport->lock, flags);
			nvmet_fc_delete_target_queue(queue);
			nvmet_fc_tgt_q_put(queue);
			spin_lock_irqsave(&tgtport->lock, flags);
		}
	}
	spin_unlock_irqrestore(&tgtport->lock, flags);

	nvmet_fc_tgt_a_put(assoc);
}