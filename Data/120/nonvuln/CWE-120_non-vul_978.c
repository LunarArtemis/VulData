static void dmar_enable_qi(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t status = 0;

	spinlock_obtain(&(dmar_unit->lock));

	dmar_unit->qi_queue = hva2hpa(get_qi_queue(dmar_unit->index));
	iommu_write64(dmar_unit, DMAR_IQA_REG, dmar_unit->qi_queue);

	iommu_write32(dmar_unit, DMAR_IQT_REG, 0U);

	if ((dmar_unit->gcmd & DMA_GCMD_QIE) == 0U) {
		dmar_unit->gcmd |= DMA_GCMD_QIE;
		iommu_write32(dmar_unit, DMAR_GCMD_REG,	dmar_unit->gcmd);
		dmar_wait_completion(dmar_unit, DMAR_GSTS_REG, DMA_GSTS_QIES, 0U, &status);
	}

	spinlock_release(&(dmar_unit->lock));
}