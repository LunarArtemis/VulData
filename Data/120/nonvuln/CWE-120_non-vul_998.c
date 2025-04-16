static void dmar_disable_qi(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t status = 0;

	spinlock_obtain(&(dmar_unit->lock));

	if ((dmar_unit->gcmd & DMA_GCMD_QIE) == DMA_GCMD_QIE) {
		dmar_unit->gcmd &= ~DMA_GCMD_QIE;
		iommu_write32(dmar_unit, DMAR_GCMD_REG,	dmar_unit->gcmd);
		dmar_wait_completion(dmar_unit, DMAR_GSTS_REG, DMA_GSTS_QIES, DMA_GSTS_QIES, &status);
	}

	spinlock_release(&(dmar_unit->lock));
}