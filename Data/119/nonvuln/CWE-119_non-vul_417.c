static void
nvmet_fc_abort_op(struct nvmet_fc_tgtport *tgtport,
				struct nvmet_fc_fcp_iod *fod)
{
	struct nvmefc_tgt_fcp_req *fcpreq = fod->fcpreq;

	/* data no longer needed */
	nvmet_fc_free_tgt_pgs(fod);

	/*
	 * if an ABTS was received or we issued the fcp_abort early
	 * don't call abort routine again.
	 */
	/* no need to take lock - lock was taken earlier to get here */
	if (!fod->aborted)
		tgtport->ops->fcp_abort(&tgtport->fc_target_port, fcpreq);

	nvmet_fc_free_fcp_iod(fod->queue, fod);
}