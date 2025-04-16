static inline bool
__nvmet_fc_fod_op_abort(struct nvmet_fc_fcp_iod *fod, bool abort)
{
	struct nvmefc_tgt_fcp_req *fcpreq = fod->fcpreq;
	struct nvmet_fc_tgtport *tgtport = fod->tgtport;

	/* if in the middle of an io and we need to tear down */
	if (abort) {
		if (fcpreq->op == NVMET_FCOP_WRITEDATA) {
			nvmet_req_complete(&fod->req, NVME_SC_INTERNAL);
			return true;
		}

		nvmet_fc_abort_op(tgtport, fod);
		return true;
	}

	return false;
}