static int32_t register_hrhd_units(void)
{
	struct dmar_drhd_rt *drhd_rt;
	uint32_t i;
	int32_t ret = 0;

	for (i = 0U; i < platform_dmar_info->drhd_count; i++) {
		drhd_rt = &dmar_drhd_units[i];
		drhd_rt->index = i;
		drhd_rt->drhd = &platform_dmar_info->drhd_units[i];
		drhd_rt->dmar_irq = IRQ_INVALID;

		set_paging_supervisor(drhd_rt->drhd->reg_base_addr, PAGE_SIZE);

		ret = dmar_register_hrhd(drhd_rt);
		if (ret != 0) {
			break;
		}

		if ((iommu_cap_pi(drhd_rt->cap) == 0U) || (!is_apicv_advanced_feature_supported())) {
			platform_caps.pi = false;
		}
	}

	return ret;
}