static uint16_t alloc_irtes(struct dmar_drhd_rt *dmar_unit, const uint16_t num)
{
	uint16_t irte_idx;
	uint64_t mask = (1UL << num) - 1U;
	uint64_t test_mask;

	ASSERT((bitmap_weight(num) == 1U) && (num <= 32U));

	spinlock_obtain(&dmar_unit->lock);
	for (irte_idx = 0U; irte_idx < CONFIG_MAX_IR_ENTRIES; irte_idx += num) {
		test_mask = mask << (irte_idx & 0x3FU);
		if ((dmar_unit->irte_alloc_bitmap[irte_idx >> 6U] & test_mask) == 0UL) {
			dmar_unit->irte_alloc_bitmap[irte_idx >> 6U] |= test_mask;
			break;
		}
	}
	spinlock_release(&dmar_unit->lock);

	return (irte_idx < CONFIG_MAX_IR_ENTRIES) ? irte_idx: INVALID_IRTE_ID;
}