static int
nvmet_fc_alloc_ls_iodlist(struct nvmet_fc_tgtport *tgtport)
{
	struct nvmet_fc_ls_iod *iod;
	int i;

	iod = kcalloc(NVMET_LS_CTX_COUNT, sizeof(struct nvmet_fc_ls_iod),
			GFP_KERNEL);
	if (!iod)
		return -ENOMEM;

	tgtport->iod = iod;

	for (i = 0; i < NVMET_LS_CTX_COUNT; iod++, i++) {
		INIT_WORK(&iod->work, nvmet_fc_handle_ls_rqst_work);
		iod->tgtport = tgtport;
		list_add_tail(&iod->ls_list, &tgtport->ls_list);

		iod->rqstbuf = kcalloc(2, NVME_FC_MAX_LS_BUFFER_SIZE,
			GFP_KERNEL);
		if (!iod->rqstbuf)
			goto out_fail;

		iod->rspbuf = iod->rqstbuf + NVME_FC_MAX_LS_BUFFER_SIZE;

		iod->rspdma = fc_dma_map_single(tgtport->dev, iod->rspbuf,
						NVME_FC_MAX_LS_BUFFER_SIZE,
						DMA_TO_DEVICE);
		if (fc_dma_mapping_error(tgtport->dev, iod->rspdma))
			goto out_fail;
	}

	return 0;

out_fail:
	kfree(iod->rqstbuf);
	list_del(&iod->ls_list);
	for (iod--, i--; i >= 0; iod--, i--) {
		fc_dma_unmap_single(tgtport->dev, iod->rspdma,
				NVME_FC_MAX_LS_BUFFER_SIZE, DMA_TO_DEVICE);
		kfree(iod->rqstbuf);
		list_del(&iod->ls_list);
	}

	kfree(iod);

	return -EFAULT;
}