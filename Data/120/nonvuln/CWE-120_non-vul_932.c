static void activate_physical_ioapic(struct acrn_vm *vm,
		struct ptirq_remapping_info *entry)
{
	union ioapic_rte rte;
	uint32_t phys_irq = entry->allocated_pirq;
	uint64_t intr_mask;
	bool is_lvl_trigger = false;

	/* disable interrupt */
	ioapic_gsi_mask_irq(phys_irq);

	/* build physical IOAPIC RTE */
	rte = ptirq_build_physical_rte(vm, entry);
	intr_mask = rte.bits.intr_mask;

	/* update irq trigger mode according to info in guest */
	if (rte.bits.trigger_mode == IOAPIC_RTE_TRGRMODE_LEVEL) {
		is_lvl_trigger = true;
	}
	set_irq_trigger_mode(phys_irq, is_lvl_trigger);

	/* set rte entry when masked */
	rte.bits.intr_mask = IOAPIC_RTE_MASK_SET;
	ioapic_set_rte(phys_irq, rte);

	if (intr_mask == IOAPIC_RTE_MASK_CLR) {
		ioapic_gsi_unmask_irq(phys_irq);
	}
}