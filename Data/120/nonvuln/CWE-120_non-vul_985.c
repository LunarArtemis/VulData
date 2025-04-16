static inline void dmar_wait_completion(const struct dmar_drhd_rt *dmar_unit, uint32_t offset,
	uint32_t mask, uint32_t pre_condition, uint32_t *status)
{
	/* variable start isn't used when built as release version */
	__unused uint64_t start = cpu_ticks();

	do {
		*status = iommu_read32(dmar_unit, offset);
		ASSERT(((cpu_ticks() - start) < TICKS_PER_MS),
			"DMAR OP Timeout!");
		asm_pause();
	} while( (*status & mask) == pre_condition);
}