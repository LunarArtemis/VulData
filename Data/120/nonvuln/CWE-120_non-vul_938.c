static void dmar_disable_intr_remapping(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t status;

	spinlock_obtain(&(dmar_unit->lock));
	if ((dmar_unit->gcmd & DMA_GCMD_IRE) != 0U) {
		dmar_unit->gcmd &= ~DMA_GCMD_IRE;
		iommu_write32(dmar_unit, DMAR_GCMD_REG, dmar_unit->gcmd);
		/* 32-bit register */
		dmar_wait_completion(dmar_unit, DMAR_GSTS_REG, DMA_GSTS_IRES, DMA_GSTS_IRES, &status);
	}

	spinlock_release(&(dmar_unit->lock));
}