static uint64_t iommu_read64(const struct dmar_drhd_rt *dmar_unit, uint32_t offset)
{
	return mmio_read64(hpa2hva(dmar_unit->drhd->reg_base_addr + offset));
}