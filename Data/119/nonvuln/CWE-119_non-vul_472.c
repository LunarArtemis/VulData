static void
nvmet_fc_handle_fcp_rqst_work(struct work_struct *work)
{
	struct nvmet_fc_fcp_iod *fod =
		container_of(work, struct nvmet_fc_fcp_iod, work);
	struct nvmet_fc_tgtport *tgtport = fod->tgtport;

	nvmet_fc_handle_fcp_rqst(tgtport, fod);
}