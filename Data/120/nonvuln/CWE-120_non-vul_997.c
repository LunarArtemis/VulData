static void enable_dmar(struct dmar_drhd_rt *dmar_unit)
{
	dev_dbg(DBG_LEVEL_IOMMU, "enable dmar uint [0x%x]", dmar_unit->drhd->reg_base_addr);
	dmar_invalid_context_cache_global(dmar_unit);
	dmar_invalid_iotlb_global(dmar_unit);
	dmar_invalid_iec_global(dmar_unit);
	dmar_enable_translation(dmar_unit);
}