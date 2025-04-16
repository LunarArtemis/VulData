static void dmar_invalid_iotlb_global(struct dmar_drhd_rt *dmar_unit)
{
	dmar_invalid_iotlb(dmar_unit, 0U, 0UL, 0U, false, DMAR_IIRG_GLOBAL);
}