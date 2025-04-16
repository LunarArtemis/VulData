static void
nvmet_fc_handle_ls_rqst_work(struct work_struct *work)
{
	struct nvmet_fc_ls_iod *iod =
		container_of(work, struct nvmet_fc_ls_iod, work);
	struct nvmet_fc_tgtport *tgtport = iod->tgtport;

	nvmet_fc_handle_ls_rqst(tgtport, iod);
}