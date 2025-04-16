static void
nvmet_fc_free_ls_iodlist(struct nvmet_fc_tgtport *tgtport)
{
	struct nvmet_fc_ls_iod *iod = tgtport->iod;
	int i;

	for (i = 0; i < NVMET_LS_CTX_COUNT; iod++, i++) {
		fc_dma_unmap_single(tgtport->dev,
				iod->rspdma, NVME_FC_MAX_LS_BUFFER_SIZE,
				DMA_TO_DEVICE);
		kfree(iod->rqstbuf);
		list_del(&iod->ls_list);
	}
	kfree(tgtport->iod);
}