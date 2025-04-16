static int
nvmet_fc_tgt_q_get(struct nvmet_fc_tgt_queue *queue)
{
	return kref_get_unless_zero(&queue->ref);
}