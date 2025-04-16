static void
nvmet_fc_fod_op_done(struct nvmet_fc_fcp_iod *fod)
{
	struct nvmefc_tgt_fcp_req *fcpreq = fod->fcpreq;
	struct nvmet_fc_tgtport *tgtport = fod->tgtport;
	unsigned long flags;
	bool abort;

	spin_lock_irqsave(&fod->flock, flags);
	abort = fod->abort;
	fod->writedataactive = false;
	spin_unlock_irqrestore(&fod->flock, flags);

	switch (fcpreq->op) {

	case NVMET_FCOP_WRITEDATA:
		if (__nvmet_fc_fod_op_abort(fod, abort))
			return;
		if (fcpreq->fcp_error ||
		    fcpreq->transferred_length != fcpreq->transfer_length) {
			spin_lock(&fod->flock);
			fod->abort = true;
			spin_unlock(&fod->flock);

			nvmet_req_complete(&fod->req, NVME_SC_INTERNAL);
			return;
		}

		fod->offset += fcpreq->transferred_length;
		if (fod->offset != fod->total_length) {
			spin_lock_irqsave(&fod->flock, flags);
			fod->writedataactive = true;
			spin_unlock_irqrestore(&fod->flock, flags);

			/* transfer the next chunk */
			nvmet_fc_transfer_fcp_data(tgtport, fod,
						NVMET_FCOP_WRITEDATA);
			return;
		}

		/* data transfer complete, resume with nvmet layer */

		fod->req.execute(&fod->req);

		break;

	case NVMET_FCOP_READDATA:
	case NVMET_FCOP_READDATA_RSP:
		if (__nvmet_fc_fod_op_abort(fod, abort))
			return;
		if (fcpreq->fcp_error ||
		    fcpreq->transferred_length != fcpreq->transfer_length) {
			nvmet_fc_abort_op(tgtport, fod);
			return;
		}

		/* success */

		if (fcpreq->op == NVMET_FCOP_READDATA_RSP) {
			/* data no longer needed */
			nvmet_fc_free_tgt_pgs(fod);
			nvmet_fc_free_fcp_iod(fod->queue, fod);
			return;
		}

		fod->offset += fcpreq->transferred_length;
		if (fod->offset != fod->total_length) {
			/* transfer the next chunk */
			nvmet_fc_transfer_fcp_data(tgtport, fod,
						NVMET_FCOP_READDATA);
			return;
		}

		/* data transfer complete, send response */

		/* data no longer needed */
		nvmet_fc_free_tgt_pgs(fod);

		nvmet_fc_xmt_fcp_rsp(tgtport, fod);

		break;

	case NVMET_FCOP_RSP:
		if (__nvmet_fc_fod_op_abort(fod, abort))
			return;
		nvmet_fc_free_fcp_iod(fod->queue, fod);
		break;

	default:
		break;
	}
}