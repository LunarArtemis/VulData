static void
nvmet_fc_free_tgt_pgs(struct nvmet_fc_fcp_iod *fod)
{
	struct scatterlist *sg;
	int count;

	if (!fod->data_sg || !fod->data_sg_cnt)
		return;

	fc_dma_unmap_sg(fod->tgtport->dev, fod->data_sg, fod->data_sg_cnt,
				((fod->io_dir == NVMET_FCP_WRITE) ?
					DMA_FROM_DEVICE : DMA_TO_DEVICE));
	for_each_sg(fod->data_sg, sg, fod->data_sg_cnt, count)
		__free_page(sg_page(sg));
	kfree(fod->data_sg);
	fod->data_sg = NULL;
	fod->data_sg_cnt = 0;
}