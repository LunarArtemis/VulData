static void ptirq_build_physical_msi(struct acrn_vm *vm,
	struct ptirq_remapping_info *entry, uint32_t vector, uint64_t pid_paddr, uint16_t irte_idx)
{
	uint64_t vdmask, pdmask;
	uint32_t dest, delmode, dest_mask;
	bool phys;
	union dmar_ir_entry irte;
	union irte_index ir_index;
	int32_t ret;
	struct intr_source intr_src;

	/* get physical destination cpu mask */
	dest = entry->vmsi.addr.bits.dest_field;
	phys = (entry->vmsi.addr.bits.dest_mode == MSI_ADDR_DESTMODE_PHYS);

	vdmask = vlapic_calc_dest_noshort(vm, false, dest, phys, false);
	pdmask = vcpumask2pcpumask(vm, vdmask);

	/* get physical delivery mode */
	delmode = entry->vmsi.data.bits.delivery_mode;
	if ((delmode != MSI_DATA_DELMODE_FIXED) && (delmode != MSI_DATA_DELMODE_LOPRI)) {
		delmode = MSI_DATA_DELMODE_LOPRI;
	}

	dest_mask = calculate_logical_dest_mask(pdmask);

	/* Using phys_irq as index in the corresponding IOMMU */
	irte.value.lo_64 = 0UL;
	irte.value.hi_64 = 0UL;
	irte.bits.remap.vector = vector;
	irte.bits.remap.delivery_mode = delmode;
	irte.bits.remap.dest_mode = MSI_ADDR_DESTMODE_LOGICAL;
	irte.bits.remap.rh = MSI_ADDR_RH;
	irte.bits.remap.dest = dest_mask;

	intr_src.is_msi = true;
	intr_src.pid_paddr = pid_paddr;
	intr_src.src.msi.value = entry->phys_sid.msi_id.bdf;
	if (entry->irte_idx == INVALID_IRTE_ID) {
		entry->irte_idx = irte_idx;
	}
	ret = dmar_assign_irte(&intr_src, &irte, entry->irte_idx, &ir_index.index);

	if (ret == 0) {
		entry->pmsi.data.full = 0U;
		entry->pmsi.addr.full = 0UL;
		entry->irte_idx = ir_index.index;
		if (ir_index.index != INVALID_IRTE_ID) {
			/*
			 * Update the MSI interrupt source to point to the IRTE
			 * SHV is set to 0 as ACRN disables MMC (Multi-Message Capable
			 * for MSI devices.
			 */
			entry->pmsi.addr.ir_bits.intr_index_high = ir_index.bits.index_high;
			entry->pmsi.addr.ir_bits.shv = 0U;
			entry->pmsi.addr.ir_bits.intr_format = 0x1U;
			entry->pmsi.addr.ir_bits.intr_index_low = ir_index.bits.index_low;
			entry->pmsi.addr.ir_bits.constant = 0xFEEU;
		}
	} else {
		/* In case there is no corresponding IOMMU, for example, if the
		 * IOMMU is ignored, pass the MSI info in Compatibility Format
		 */
		entry->pmsi.data = entry->vmsi.data;
		entry->pmsi.data.bits.delivery_mode = delmode;
		entry->pmsi.data.bits.vector = vector;

		entry->pmsi.addr = entry->vmsi.addr;
		entry->pmsi.addr.bits.dest_field = dest_mask;
		entry->pmsi.addr.bits.rh = MSI_ADDR_RH;
		entry->pmsi.addr.bits.dest_mode = MSI_ADDR_DESTMODE_LOGICAL;
	}
	dev_dbg(DBG_LEVEL_IRQ, "MSI %s addr:data = 0x%lx:%x(V) -> 0x%lx:%x(P)",
		(entry->pmsi.addr.ir_bits.intr_format != 0U) ? " Remappable Format" : "Compatibility Format",
		entry->vmsi.addr.full, entry->vmsi.data.full,
		entry->pmsi.addr.full, entry->pmsi.data.full);
}