void ptirq_intx_ack(struct acrn_vm *vm, uint32_t virt_gsi, enum intx_ctlr vgsi_ctlr)
{
	uint32_t phys_irq;
	struct ptirq_remapping_info *entry;
	DEFINE_INTX_SID(virt_sid, virt_gsi, vgsi_ctlr);

	entry = find_ptirq_entry(PTDEV_INTR_INTX, &virt_sid, vm);
	if (entry != NULL) {
		phys_irq = entry->allocated_pirq;

		/* NOTE: only Level trigger will process EOI/ACK and if we got here
		 * means we have this vioapic or vpic or both enabled
		 */
		switch (vgsi_ctlr) {
		case INTX_CTLR_IOAPIC:
			if (entry->polarity != 0U) {
				vioapic_set_irqline_lock(vm, virt_gsi, GSI_SET_HIGH);
			} else {
				vioapic_set_irqline_lock(vm, virt_gsi, GSI_SET_LOW);
			}
			break;
		case INTX_CTLR_PIC:
			vpic_set_irqline(vm_pic(vm), virt_gsi, GSI_SET_LOW);
			break;
		default:
			/*
			 * In this switch statement, vgsi_ctlr shall either be
			 * INTX_CTLR_IOAPIC or INTX_CTLR_PIC.
			 * Gracefully return if prior case clauses have not been met.
			 */
			break;
		}

		dev_dbg(DBG_LEVEL_PTIRQ, "dev-assign: irq=0x%x acked vr: 0x%x",
				phys_irq, irq_to_vector(phys_irq));
		ioapic_gsi_unmask_irq(phys_irq);
	}
}