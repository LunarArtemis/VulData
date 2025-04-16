static void
nvmet_fc_transfer_fcp_data(struct nvmet_fc_tgtport *tgtport,
				struct nvmet_fc_fcp_iod *fod, u8 op)
{
	struct nvmefc_tgt_fcp_req *fcpreq = fod->fcpreq;
	unsigned long flags;
	u32 tlen;
	int ret;

	fcpreq->op = op;
	fcpreq->offset = fod->offset;
	fcpreq->timeout = NVME_FC_TGTOP_TIMEOUT_SEC;

	tlen = min_t(u32, tgtport->max_sg_cnt * PAGE_SIZE,
			(fod->total_length - fod->offset));
	fcpreq->transfer_length = tlen;
	fcpreq->transferred_length = 0;
	fcpreq->fcp_error = 0;
	fcpreq->rsplen = 0;

	fcpreq->sg = &fod->data_sg[fod->offset / PAGE_SIZE];
	fcpreq->sg_cnt = DIV_ROUND_UP(tlen, PAGE_SIZE);

	/*
	 * If the last READDATA request: check if LLDD supports
	 * combined xfr with response.
	 */
	if ((op == NVMET_FCOP_READDATA) &&
	    ((fod->offset + fcpreq->transfer_length) == fod->total_length) &&
	    (tgtport->ops->target_features & NVMET_FCTGTFEAT_READDATA_RSP)) {
		fcpreq->op = NVMET_FCOP_READDATA_RSP;
		nvmet_fc_prep_fcp_rsp(tgtport, fod);
	}

	ret = tgtport->ops->fcp_op(&tgtport->fc_target_port, fod->fcpreq);
	if (ret) {
		/*
		 * should be ok to set w/o lock as its in the thread of
		 * execution (not an async timer routine) and doesn't
		 * contend with any clearing action
		 */
		fod->abort = true;

		if (op == NVMET_FCOP_WRITEDATA) {
			spin_lock_irqsave(&fod->flock, flags);
			fod->writedataactive = false;
			spin_unlock_irqrestore(&fod->flock, flags);
			nvmet_req_complete(&fod->req, NVME_SC_INTERNAL);
		} else /* NVMET_FCOP_READDATA or NVMET_FCOP_READDATA_RSP */ {
			fcpreq->fcp_error = ret;
			fcpreq->transferred_length = 0;
			nvmet_fc_xmt_fcp_op_done(fod->fcpreq);
		}
	}
}