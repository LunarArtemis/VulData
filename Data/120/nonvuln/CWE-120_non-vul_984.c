static void dmar_enable_intr_remapping(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t status = 0;

	spinlock_obtain(&(dmar_unit->lock));
	if ((dmar_unit->gcmd & DMA_GCMD_IRE) == 0U) {
		dmar_unit->gcmd |= DMA_GCMD_IRE;
		iommu_write32(dmar_unit, DMAR_GCMD_REG, dmar_unit->gcmd);
		/* 32-bit register */
		dmar_wait_completion(dmar_unit, DMAR_GSTS_REG, DMA_GSTS_IRES, 0U, &status);
#if DBG_IOMMU
		status = iommu_read32(dmar_unit, DMAR_GSTS_REG);
#endif
	}

	spinlock_release(&(dmar_unit->lock));
	dev_dbg(DBG_LEVEL_IOMMU, "%s: gsr:0x%x", __func__, status);
}