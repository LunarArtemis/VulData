static uint8_t dmar_unit_get_msagw(const struct dmar_drhd_rt *dmar_unit)
{
	uint8_t i;
	uint8_t sgaw = iommu_cap_sagaw(dmar_unit->cap);

	for (i = 5U; i > 0U; ) {
		i--;
		if (((1U << i) & sgaw) != 0U) {
			break;
		}
	}
	return i;
}