static void dmar_set_root_table(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t status;

	spinlock_obtain(&(dmar_unit->lock));
	iommu_write64(dmar_unit, DMAR_RTADDR_REG, dmar_unit->root_table_addr);

	iommu_write32(dmar_unit, DMAR_GCMD_REG, dmar_unit->gcmd | DMA_GCMD_SRTP);

	/* 32-bit register */
	dmar_wait_completion(dmar_unit, DMAR_GSTS_REG, DMA_GSTS_RTPS, 0U, &status);
	spinlock_release(&(dmar_unit->lock));
}