static void
nvmet_fc_tgt_queue_free(struct kref *ref)
{
	struct nvmet_fc_tgt_queue *queue =
		container_of(ref, struct nvmet_fc_tgt_queue, ref);
	unsigned long flags;

	spin_lock_irqsave(&queue->assoc->tgtport->lock, flags);
	queue->assoc->queues[queue->qid] = NULL;
	spin_unlock_irqrestore(&queue->assoc->tgtport->lock, flags);

	nvmet_fc_destroy_fcp_iodlist(queue->assoc->tgtport, queue);

	nvmet_fc_tgt_a_put(queue->assoc);

	destroy_workqueue(queue->work_q);

	kfree(queue);
}