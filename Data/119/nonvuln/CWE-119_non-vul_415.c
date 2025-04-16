static void
nvmet_fc_destroy_fcp_iodlist(struct nvmet_fc_tgtport *tgtport,
				struct nvmet_fc_tgt_queue *queue)
{
	struct nvmet_fc_fcp_iod *fod = queue->fod;
	int i;

	for (i = 0; i < queue->sqsize; fod++, i++) {
		if (fod->rspdma)
			fc_dma_unmap_single(tgtport->dev, fod->rspdma,
				sizeof(fod->rspiubuf), DMA_TO_DEVICE);
	}
}