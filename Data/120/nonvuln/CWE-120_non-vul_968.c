static void iommu_write64(const struct dmar_drhd_rt *dmar_unit, uint32_t offset, uint64_t value)
{
	mmio_write64(value, hpa2hva(dmar_unit->drhd->reg_base_addr + offset));
}