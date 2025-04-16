static void dmar_fault_event_mask(struct dmar_drhd_rt *dmar_unit)
{
	spinlock_obtain(&(dmar_unit->lock));
	iommu_write32(dmar_unit, DMAR_FECTL_REG, DMA_FECTL_IM);
	spinlock_release(&(dmar_unit->lock));
}