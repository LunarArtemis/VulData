static void
nvmet_fc_prep_fcp_iodlist(struct nvmet_fc_tgtport *tgtport,
				struct nvmet_fc_tgt_queue *queue)
{
	struct nvmet_fc_fcp_iod *fod = queue->fod;
	int i;

	for (i = 0; i < queue->sqsize; fod++, i++) {
		INIT_WORK(&fod->work, nvmet_fc_handle_fcp_rqst_work);
		INIT_WORK(&fod->done_work, nvmet_fc_fcp_rqst_op_done_work);
		fod->tgtport = tgtport;
		fod->queue = queue;
		fod->active = false;
		fod->abort = false;
		fod->aborted = false;
		fod->fcpreq = NULL;
		list_add_tail(&fod->fcp_list, &queue->fod_list);
		spin_lock_init(&fod->flock);

		fod->rspdma = fc_dma_map_single(tgtport->dev, &fod->rspiubuf,
					sizeof(fod->rspiubuf), DMA_TO_DEVICE);
		if (fc_dma_mapping_error(tgtport->dev, fod->rspdma)) {
			list_del(&fod->fcp_list);
			for (fod--, i--; i >= 0; fod--, i--) {
				fc_dma_unmap_single(tgtport->dev, fod->rspdma,
						sizeof(fod->rspiubuf),
						DMA_TO_DEVICE);
				fod->rspdma = 0L;
				list_del(&fod->fcp_list);
			}

			return;
		}
	}
}