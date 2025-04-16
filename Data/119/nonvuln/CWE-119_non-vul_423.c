static void
nvmet_fc_xmt_fcp_op_done(struct nvmefc_tgt_fcp_req *fcpreq)
{
	struct nvmet_fc_fcp_iod *fod = fcpreq->nvmet_fc_private;
	struct nvmet_fc_tgt_queue *queue = fod->queue;

	if (fod->tgtport->ops->target_features & NVMET_FCTGTFEAT_OPDONE_IN_ISR)
		/* context switch so completion is not in ISR context */
		queue_work_on(queue->cpu, queue->work_q, &fod->done_work);
	else
		nvmet_fc_fod_op_done(fod);
}