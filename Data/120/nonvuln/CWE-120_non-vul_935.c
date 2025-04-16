static void ptirq_handle_intx(struct acrn_vm *vm,
		const struct ptirq_remapping_info *entry)
{
	const union source_id *virt_sid = &entry->virt_sid;

	switch (virt_sid->intx_id.ctlr) {
	case INTX_CTLR_IOAPIC:
	{
		union ioapic_rte rte;
		bool trigger_lvl = false;

		/* INTX_CTLR_IOAPIC means we have vioapic enabled */
		vioapic_get_rte(vm, (uint32_t)virt_sid->intx_id.gsi, &rte);
		if (rte.bits.trigger_mode == IOAPIC_RTE_TRGRMODE_LEVEL) {
			trigger_lvl = true;
		}

		if (trigger_lvl) {
			if (entry->polarity != 0U) {
				vioapic_set_irqline_lock(vm, virt_sid->intx_id.gsi, GSI_SET_LOW);
			} else {
				vioapic_set_irqline_lock(vm, virt_sid->intx_id.gsi, GSI_SET_HIGH);
			}
		} else {
			if (entry->polarity != 0U) {
				vioapic_set_irqline_lock(vm, virt_sid->intx_id.gsi, GSI_FALLING_PULSE);
			} else {
				vioapic_set_irqline_lock(vm, virt_sid->intx_id.gsi, GSI_RAISING_PULSE);
			}
		}

		dev_dbg(DBG_LEVEL_PTIRQ,
			"dev-assign: irq=0x%x assert vr: 0x%x vRTE=0x%lx",
			entry->allocated_pirq,
			irq_to_vector(entry->allocated_pirq),
			rte.full);
		break;
	}
	case INTX_CTLR_PIC:
	{
		enum vpic_trigger trigger;

		/* INTX_CTLR_PIC means we have vpic enabled */
		vpic_get_irqline_trigger_mode(vm_pic(vm), virt_sid->intx_id.gsi, &trigger);
		if (trigger == LEVEL_TRIGGER) {
			vpic_set_irqline(vm_pic(vm), virt_sid->intx_id.gsi, GSI_SET_HIGH);
		} else {
			vpic_set_irqline(vm_pic(vm), virt_sid->intx_id.gsi, GSI_RAISING_PULSE);
		}
		break;
	}
	default:
		/*
		 * In this switch statement, virt_sid->intx_id.ctlr shall
		 * either be INTX_CTLR_IOAPIC or INTX_CTLR_PIC.
		 * Gracefully return if prior case clauses have not been met.
		 */
		break;
	}
}