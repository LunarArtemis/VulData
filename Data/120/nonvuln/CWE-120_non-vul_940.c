static void dmar_set_intr_remap_table(struct dmar_drhd_rt *dmar_unit)
{
	uint64_t address;
	uint32_t status;
	uint8_t size;

	spinlock_obtain(&(dmar_unit->lock));

	/* Set number of bits needed to represent the entries minus 1 */
	size = (uint8_t) fls32(CONFIG_MAX_IR_ENTRIES) - 1U;
	address = dmar_unit->ir_table_addr | DMAR_IR_ENABLE_EIM | size;

	iommu_write64(dmar_unit, DMAR_IRTA_REG, address);

	iommu_write32(dmar_unit, DMAR_GCMD_REG, dmar_unit->gcmd | DMA_GCMD_SIRTP);

	dmar_wait_completion(dmar_unit, DMAR_GSTS_REG, DMA_GSTS_IRTPS, 0U, &status);

	spinlock_release(&(dmar_unit->lock));
}