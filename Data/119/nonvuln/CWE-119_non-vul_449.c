static void
nvmet_fc_free_fcp_iod(struct nvmet_fc_tgt_queue *queue,
			struct nvmet_fc_fcp_iod *fod)
{
	struct nvmefc_tgt_fcp_req *fcpreq = fod->fcpreq;
	struct nvmet_fc_tgtport *tgtport = fod->tgtport;
	struct nvmet_fc_defer_fcp_req *deferfcp;
	unsigned long flags;

	fc_dma_sync_single_for_cpu(tgtport->dev, fod->rspdma,
				sizeof(fod->rspiubuf), DMA_TO_DEVICE);

	fcpreq->nvmet_fc_private = NULL;

	fod->active = false;
	fod->abort = false;
	fod->aborted = false;
	fod->writedataactive = false;
	fod->fcpreq = NULL;

	tgtport->ops->fcp_req_release(&tgtport->fc_target_port, fcpreq);

	spin_lock_irqsave(&queue->qlock, flags);
	deferfcp = list_first_entry_or_null(&queue->pending_cmd_list,
				struct nvmet_fc_defer_fcp_req, req_list);
	if (!deferfcp) {
		list_add_tail(&fod->fcp_list, &fod->queue->fod_list);
		spin_unlock_irqrestore(&queue->qlock, flags);

		/* Release reference taken at queue lookup and fod allocation */
		nvmet_fc_tgt_q_put(queue);
		return;
	}

	/* Re-use the fod for the next pending cmd that was deferred */
	list_del(&deferfcp->req_list);

	fcpreq = deferfcp->fcp_req;

	/* deferfcp can be reused for another IO at a later date */
	list_add_tail(&deferfcp->req_list, &queue->avail_defer_list);

	spin_unlock_irqrestore(&queue->qlock, flags);

	/* Save NVME CMD IO in fod */
	memcpy(&fod->cmdiubuf, fcpreq->rspaddr, fcpreq->rsplen);

	/* Setup new fcpreq to be processed */
	fcpreq->rspaddr = NULL;
	fcpreq->rsplen  = 0;
	fcpreq->nvmet_fc_private = fod;
	fod->fcpreq = fcpreq;
	fod->active = true;

	/* inform LLDD IO is now being processed */
	tgtport->ops->defer_rcv(&tgtport->fc_target_port, fcpreq);

	/* Submit deferred IO for processing */
	nvmet_fc_queue_fcp_req(tgtport, queue, fcpreq);

	/*
	 * Leave the queue lookup get reference taken when
	 * fod was originally allocated.
	 */
}