static void
nvmet_fc_handle_fcp_rqst(struct nvmet_fc_tgtport *tgtport,
			struct nvmet_fc_fcp_iod *fod)
{
	struct nvme_fc_cmd_iu *cmdiu = &fod->cmdiubuf;
	int ret;

	/*
	 * Fused commands are currently not supported in the linux
	 * implementation.
	 *
	 * As such, the implementation of the FC transport does not
	 * look at the fused commands and order delivery to the upper
	 * layer until we have both based on csn.
	 */

	fod->fcpreq->done = nvmet_fc_xmt_fcp_op_done;

	fod->total_length = be32_to_cpu(cmdiu->data_len);
	if (cmdiu->flags & FCNVME_CMD_FLAGS_WRITE) {
		fod->io_dir = NVMET_FCP_WRITE;
		if (!nvme_is_write(&cmdiu->sqe))
			goto transport_error;
	} else if (cmdiu->flags & FCNVME_CMD_FLAGS_READ) {
		fod->io_dir = NVMET_FCP_READ;
		if (nvme_is_write(&cmdiu->sqe))
			goto transport_error;
	} else {
		fod->io_dir = NVMET_FCP_NODATA;
		if (fod->total_length)
			goto transport_error;
	}

	fod->req.cmd = &fod->cmdiubuf.sqe;
	fod->req.rsp = &fod->rspiubuf.cqe;
	fod->req.port = fod->queue->port;

	/* ensure nvmet handlers will set cmd handler callback */
	fod->req.execute = NULL;

	/* clear any response payload */
	memset(&fod->rspiubuf, 0, sizeof(fod->rspiubuf));

	fod->data_sg = NULL;
	fod->data_sg_cnt = 0;

	ret = nvmet_req_init(&fod->req,
				&fod->queue->nvme_cq,
				&fod->queue->nvme_sq,
				&nvmet_fc_tgt_fcp_ops);
	if (!ret) {
		/* bad SQE content or invalid ctrl state */
		/* nvmet layer has already called op done to send rsp. */
		return;
	}

	/* keep a running counter of tail position */
	atomic_inc(&fod->queue->sqtail);

	if (fod->total_length) {
		ret = nvmet_fc_alloc_tgt_pgs(fod);
		if (ret) {
			nvmet_req_complete(&fod->req, ret);
			return;
		}
	}
	fod->req.sg = fod->data_sg;
	fod->req.sg_cnt = fod->data_sg_cnt;
	fod->offset = 0;

	if (fod->io_dir == NVMET_FCP_WRITE) {
		/* pull the data over before invoking nvmet layer */
		nvmet_fc_transfer_fcp_data(tgtport, fod, NVMET_FCOP_WRITEDATA);
		return;
	}

	/*
	 * Reads or no data:
	 *
	 * can invoke the nvmet_layer now. If read data, cmd completion will
	 * push the data
	 */

	fod->req.execute(&fod->req);

	return;

transport_error:
	nvmet_fc_abort_op(tgtport, fod);
}