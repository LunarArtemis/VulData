static int
nvmet_fc_alloc_tgt_pgs(struct nvmet_fc_fcp_iod *fod)
{
	struct scatterlist *sg;
	struct page *page;
	unsigned int nent;
	u32 page_len, length;
	int i = 0;

	length = fod->total_length;
	nent = DIV_ROUND_UP(length, PAGE_SIZE);
	sg = kmalloc_array(nent, sizeof(struct scatterlist), GFP_KERNEL);
	if (!sg)
		goto out;

	sg_init_table(sg, nent);

	while (length) {
		page_len = min_t(u32, length, PAGE_SIZE);

		page = alloc_page(GFP_KERNEL);
		if (!page)
			goto out_free_pages;

		sg_set_page(&sg[i], page, page_len, 0);
		length -= page_len;
		i++;
	}

	fod->data_sg = sg;
	fod->data_sg_cnt = nent;
	fod->data_sg_cnt = fc_dma_map_sg(fod->tgtport->dev, sg, nent,
				((fod->io_dir == NVMET_FCP_WRITE) ?
					DMA_FROM_DEVICE : DMA_TO_DEVICE));
				/* note: write from initiator perspective */

	return 0;

out_free_pages:
	while (i > 0) {
		i--;
		__free_page(sg_page(&sg[i]));
	}
	kfree(sg);
	fod->data_sg = NULL;
	fod->data_sg_cnt = 0;
out:
	return NVME_SC_INTERNAL;
}