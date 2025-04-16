static void prepare_dmar(struct dmar_drhd_rt *dmar_unit)
{
	dev_dbg(DBG_LEVEL_IOMMU, "enable dmar uint [0x%x]", dmar_unit->drhd->reg_base_addr);
	dmar_setup_interrupt(dmar_unit);
	dmar_set_root_table(dmar_unit);
	dmar_enable_qi(dmar_unit);
	dmar_set_intr_remap_table(dmar_unit);
}