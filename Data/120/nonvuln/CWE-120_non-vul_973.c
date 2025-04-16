static void dmar_invalid_iec_global(struct dmar_drhd_rt *dmar_unit)
{
	dmar_invalid_iec(dmar_unit, 0U, 0U, true);
}