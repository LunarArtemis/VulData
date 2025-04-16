static void
nvmet_fc_fcp_rqst_op_done_work(struct work_struct *work)
{
	struct nvmet_fc_fcp_iod *fod =
		container_of(work, struct nvmet_fc_fcp_iod, done_work);

	nvmet_fc_fod_op_done(fod);
}