static void
nvmet_fc_queue_fcp_req(struct nvmet_fc_tgtport *tgtport,
		       struct nvmet_fc_tgt_queue *queue,
		       struct nvmefc_tgt_fcp_req *fcpreq)
{
	struct nvmet_fc_fcp_iod *fod = fcpreq->nvmet_fc_private;

	/*
	 * put all admin cmds on hw queue id 0. All io commands go to
	 * the respective hw queue based on a modulo basis
	 */
	fcpreq->hwqid = queue->qid ?
			((queue->qid - 1) % tgtport->ops->max_hw_queues) : 0;

	if (tgtport->ops->target_features & NVMET_FCTGTFEAT_CMD_IN_ISR)
		queue_work_on(queue->cpu, queue->work_q, &fod->work);
	else
		nvmet_fc_handle_fcp_rqst(tgtport, fod);
}