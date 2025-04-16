static void disable_dmar(struct dmar_drhd_rt *dmar_unit)
{
	dmar_disable_qi(dmar_unit);
	dmar_disable_translation(dmar_unit);
	dmar_fault_event_mask(dmar_unit);
	dmar_disable_intr_remapping(dmar_unit);
}