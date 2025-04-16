int
nvmet_fc_rcv_fcp_req(struct nvmet_fc_target_port *target_port,
			struct nvmefc_tgt_fcp_req *fcpreq,
			void *cmdiubuf, u32 cmdiubuf_len)
{
	struct nvmet_fc_tgtport *tgtport = targetport_to_tgtport(target_port);
	struct nvme_fc_cmd_iu *cmdiu = cmdiubuf;
	struct nvmet_fc_tgt_queue *queue;
	struct nvmet_fc_fcp_iod *fod;
	struct nvmet_fc_defer_fcp_req *deferfcp;
	unsigned long flags;

	/* validate iu, so the connection id can be used to find the queue */
	if ((cmdiubuf_len != sizeof(*cmdiu)) ||
			(cmdiu->scsi_id != NVME_CMD_SCSI_ID) ||
			(cmdiu->fc_id != NVME_CMD_FC_ID) ||
			(be16_to_cpu(cmdiu->iu_len) != (sizeof(*cmdiu)/4)))
		return -EIO;

	queue = nvmet_fc_find_target_queue(tgtport,
				be64_to_cpu(cmdiu->connection_id));
	if (!queue)
		return -ENOTCONN;

	/*
	 * note: reference taken by find_target_queue
	 * After successful fod allocation, the fod will inherit the
	 * ownership of that reference and will remove the reference
	 * when the fod is freed.
	 */

	spin_lock_irqsave(&queue->qlock, flags);

	fod = nvmet_fc_alloc_fcp_iod(queue);
	if (fod) {
		spin_unlock_irqrestore(&queue->qlock, flags);

		fcpreq->nvmet_fc_private = fod;
		fod->fcpreq = fcpreq;

		memcpy(&fod->cmdiubuf, cmdiubuf, cmdiubuf_len);

		nvmet_fc_queue_fcp_req(tgtport, queue, fcpreq);

		return 0;
	}

	if (!tgtport->ops->defer_rcv) {
		spin_unlock_irqrestore(&queue->qlock, flags);
		/* release the queue lookup reference */
		nvmet_fc_tgt_q_put(queue);
		return -ENOENT;
	}

	deferfcp = list_first_entry_or_null(&queue->avail_defer_list,
			struct nvmet_fc_defer_fcp_req, req_list);
	if (deferfcp) {
		/* Just re-use one that was previously allocated */
		list_del(&deferfcp->req_list);
	} else {
		spin_unlock_irqrestore(&queue->qlock, flags);

		/* Now we need to dynamically allocate one */
		deferfcp = kmalloc(sizeof(*deferfcp), GFP_KERNEL);
		if (!deferfcp) {
			/* release the queue lookup reference */
			nvmet_fc_tgt_q_put(queue);
			return -ENOMEM;
		}
		spin_lock_irqsave(&queue->qlock, flags);
	}

	/* For now, use rspaddr / rsplen to save payload information */
	fcpreq->rspaddr = cmdiubuf;
	fcpreq->rsplen  = cmdiubuf_len;
	deferfcp->fcp_req = fcpreq;

	/* defer processing till a fod becomes available */
	list_add_tail(&deferfcp->req_list, &queue->pending_cmd_list);

	/* NOTE: the queue lookup reference is still valid */

	spin_unlock_irqrestore(&queue->qlock, flags);

	return -EOVERFLOW;
}