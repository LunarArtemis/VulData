int32_t dmar_reserve_irte(const struct intr_source *intr_src, uint16_t num, uint16_t *start_id)
{
	struct dmar_drhd_rt *dmar_unit;
	union pci_bdf sid;
	uint64_t mask = (1UL << num) - 1U;
	int32_t ret = -EINVAL;

	if (intr_src->is_msi) {
		dmar_unit = device_to_dmaru((uint8_t)intr_src->src.msi.bits.b, intr_src->src.msi.fields.devfun);
		sid.value = (uint16_t)(intr_src->src.msi.value);
	} else {
		dmar_unit = ioapic_to_dmaru(intr_src->src.ioapic_id, &sid);
	}

	if (is_dmar_unit_valid(dmar_unit, sid)) {
		*start_id = alloc_irtes(dmar_unit, num);
		if (*start_id < CONFIG_MAX_IR_ENTRIES) {
			dmar_unit->irte_reserved_bitmap[*start_id >> 6U] |= mask << (*start_id & 0x3FU);
		}
		ret = 0;
	}

	pr_dbg("%s: for dev 0x%x:%x.%x, reserve %u entry for MSI(%d), start from %d",
		__func__, sid.bits.b, sid.bits.d, sid.bits.f, num, intr_src->is_msi, *start_id);
	return ret;
}