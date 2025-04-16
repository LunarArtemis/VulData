static void
nvmet_fc_xmt_ls_rsp(struct nvmet_fc_tgtport *tgtport,
				struct nvmet_fc_ls_iod *iod)
{
	int ret;

	fc_dma_sync_single_for_device(tgtport->dev, iod->rspdma,
				  NVME_FC_MAX_LS_BUFFER_SIZE, DMA_TO_DEVICE);

	ret = tgtport->ops->xmt_ls_rsp(&tgtport->fc_target_port, iod->lsreq);
	if (ret)
		nvmet_fc_xmt_ls_rsp_done(iod->lsreq);
}