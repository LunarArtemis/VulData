static void remove_intx_remapping(const struct acrn_vm *vm, uint32_t virt_gsi, enum intx_ctlr vgsi_ctlr)
{
	uint32_t phys_irq;
	struct ptirq_remapping_info *entry;
	struct intr_source intr_src;
	DEFINE_INTX_SID(virt_sid, virt_gsi, vgsi_ctlr);

	entry = find_ptirq_entry(PTDEV_INTR_INTX, &virt_sid, vm);
	if (entry != NULL) {
		if (is_entry_active(entry)) {
			phys_irq = entry->allocated_pirq;
			/* disable interrupt */
			ioapic_gsi_mask_irq(phys_irq);

			ptirq_deactivate_entry(entry);
			intr_src.is_msi = false;
			intr_src.src.ioapic_id = ioapic_irq_to_ioapic_id(phys_irq);

			dmar_free_irte(&intr_src, entry->irte_idx);
			dev_dbg(DBG_LEVEL_IRQ,
				"deactive %s intx entry:pgsi=%d, pirq=%d ",
				(vgsi_ctlr == INTX_CTLR_PIC) ? "vPIC" : "vIOAPIC",
				entry->phys_sid.intx_id.gsi, phys_irq);
			dev_dbg(DBG_LEVEL_IRQ, "from vm%d vgsi=%d\n",
				entry->vm->vm_id, virt_gsi);
		}

		ptirq_release_entry(entry);
	}
}