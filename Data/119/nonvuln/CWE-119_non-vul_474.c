static void
nvmet_fc_tgt_q_put(struct nvmet_fc_tgt_queue *queue)
{
	kref_put(&queue->ref, nvmet_fc_tgt_queue_free);
}