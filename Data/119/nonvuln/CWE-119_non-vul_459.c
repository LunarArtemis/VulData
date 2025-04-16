static void
nvmet_fc_xmt_ls_rsp_done(struct nvmefc_tgt_ls_req *lsreq)
{
	struct nvmet_fc_ls_iod *iod = lsreq->nvmet_fc_private;
	struct nvmet_fc_tgtport *tgtport = iod->tgtport;

	fc_dma_sync_single_for_cpu(tgtport->dev, iod->rspdma,
				NVME_FC_MAX_LS_BUFFER_SIZE, DMA_TO_DEVICE);
	nvmet_fc_free_ls_iod(tgtport, iod);
	nvmet_fc_tgtport_put(tgtport);
}