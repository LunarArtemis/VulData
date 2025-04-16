static void resume_dmar(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t i;

	/* restore IOMMU fault register state */
	for (i = 0U; i < IOMMU_FAULT_REGISTER_STATE_NUM; i++) {
		iommu_write32(dmar_unit, DMAR_FECTL_REG + (i * IOMMU_FAULT_REGISTER_SIZE), dmar_unit->fault_state[i]);
	}
	prepare_dmar(dmar_unit);
	enable_dmar(dmar_unit);
	dmar_enable_intr_remapping(dmar_unit);
}