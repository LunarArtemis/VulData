int32_t init_iommu(void)
{
	int32_t ret = 0;

	platform_dmar_info = get_dmar_info();

	if ((platform_dmar_info == NULL) || (platform_dmar_info->drhd_count == 0U)) {
		pr_fatal("%s: can't find dmar info\n", __func__);
		ret = -ENODEV;
	} else if (platform_dmar_info->drhd_count > CONFIG_MAX_IOMMU_NUM) {
		pr_fatal("%s: dmar count(%d) beyond the limitation(%d)\n",
				__func__, platform_dmar_info->drhd_count, CONFIG_MAX_IOMMU_NUM);
		ret = -EINVAL;
	} else {
		ret = register_hrhd_units();
		if (ret == 0) {
			do_action_for_iommus(prepare_dmar);
		}
	}
	return ret;
}