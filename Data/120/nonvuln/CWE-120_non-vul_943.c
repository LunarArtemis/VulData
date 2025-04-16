static void do_action_for_iommus(void (*action)(struct dmar_drhd_rt *))
{
	struct dmar_drhd_rt *dmar_unit;
	uint32_t i;

	for (i = 0U; i < platform_dmar_info->drhd_count; i++) {
		dmar_unit = &dmar_drhd_units[i];
		if (!dmar_unit->drhd->ignore) {
			action(dmar_unit);
		} else {
			dev_dbg(DBG_LEVEL_IOMMU, "ignore dmar_unit @0x%x", dmar_unit->drhd->reg_base_addr);
		}
	}
}