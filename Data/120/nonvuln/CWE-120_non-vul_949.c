int32_t dmar_assign_irte(const struct intr_source *intr_src, union dmar_ir_entry *irte,
	uint16_t idx_in, uint16_t *idx_out)
{
	struct dmar_drhd_rt *dmar_unit;
	union dmar_ir_entry *ir_table, *ir_entry;
	union pci_bdf sid;
	uint64_t trigger_mode;
	int32_t ret = -EINVAL;

	if (intr_src->is_msi) {
		dmar_unit = device_to_dmaru((uint8_t)intr_src->src.msi.bits.b, intr_src->src.msi.fields.devfun);
		sid.value = (uint16_t)(intr_src->src.msi.value);
		trigger_mode = 0x0UL;
	} else {
		dmar_unit = ioapic_to_dmaru(intr_src->src.ioapic_id, &sid);
		trigger_mode = irte->bits.remap.trigger_mode;
	}

	if (is_dmar_unit_valid(dmar_unit, sid)) {
		dmar_enable_intr_remapping(dmar_unit);

		ir_table = (union dmar_ir_entry *)hpa2hva(dmar_unit->ir_table_addr);
		*idx_out = idx_in;
		if (idx_in == INVALID_IRTE_ID) {
			*idx_out = alloc_irtes(dmar_unit, 1U);
		}
		if (*idx_out < CONFIG_MAX_IR_ENTRIES) {
			ir_entry = ir_table + *idx_out;

			if (intr_src->pid_paddr != 0UL) {
				union dmar_ir_entry irte_pi;

				/* irte is in remapped mode format, convert to posted mode format */
				irte_pi.value.lo_64 = 0UL;
				irte_pi.value.hi_64 = 0UL;

				irte_pi.bits.post.vector = irte->bits.remap.vector;

				irte_pi.bits.post.svt = 0x1UL;
				irte_pi.bits.post.sid = sid.value;
				irte_pi.bits.post.present = 0x1UL;
				irte_pi.bits.post.mode = 0x1UL;

				irte_pi.bits.post.pda_l = (intr_src->pid_paddr) >> 6U;
				irte_pi.bits.post.pda_h = (intr_src->pid_paddr) >> 32U;

				*ir_entry = irte_pi;
			} else {
				/* Fields that have not been initialized explicitly default to 0 */
				irte->bits.remap.svt = 0x1UL;
				irte->bits.remap.sid = sid.value;
				irte->bits.remap.present = 0x1UL;
				irte->bits.remap.trigger_mode = trigger_mode;

				*ir_entry = *irte;
			}
			iommu_flush_cache(ir_entry, sizeof(union dmar_ir_entry));
			dmar_invalid_iec(dmar_unit, *idx_out, 0U, false);
		}
		ret = 0;
	}

	return ret;
}