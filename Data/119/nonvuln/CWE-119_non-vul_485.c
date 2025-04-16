static void
__nvmet_fc_fcp_nvme_cmd_done(struct nvmet_fc_tgtport *tgtport,
			struct nvmet_fc_fcp_iod *fod, int status)
{
	struct nvme_common_command *sqe = &fod->cmdiubuf.sqe.common;
	struct nvme_completion *cqe = &fod->rspiubuf.cqe;
	unsigned long flags;
	bool abort;

	spin_lock_irqsave(&fod->flock, flags);
	abort = fod->abort;
	spin_unlock_irqrestore(&fod->flock, flags);

	/* if we have a CQE, snoop the last sq_head value */
	if (!status)
		fod->queue->sqhd = cqe->sq_head;

	if (abort) {
		nvmet_fc_abort_op(tgtport, fod);
		return;
	}

	/* if an error handling the cmd post initial parsing */
	if (status) {
		/* fudge up a failed CQE status for our transport error */
		memset(cqe, 0, sizeof(*cqe));
		cqe->sq_head = fod->queue->sqhd;	/* echo last cqe sqhd */
		cqe->sq_id = cpu_to_le16(fod->queue->qid);
		cqe->command_id = sqe->command_id;
		cqe->status = cpu_to_le16(status);
	} else {

		/*
		 * try to push the data even if the SQE status is non-zero.
		 * There may be a status where data still was intended to
		 * be moved
		 */
		if ((fod->io_dir == NVMET_FCP_READ) && (fod->data_sg_cnt)) {
			/* push the data over before sending rsp */
			nvmet_fc_transfer_fcp_data(tgtport, fod,
						NVMET_FCOP_READDATA);
			return;
		}

		/* writes & no data - fall thru */
	}

	/* data no longer needed */
	nvmet_fc_free_tgt_pgs(fod);

	nvmet_fc_xmt_fcp_rsp(tgtport, fod);
}