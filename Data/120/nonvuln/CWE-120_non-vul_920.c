static union ioapic_rte
ptirq_build_physical_rte(struct acrn_vm *vm, struct ptirq_remapping_info *entry)
{
	union ioapic_rte rte;
	uint32_t phys_irq = entry->allocated_pirq;
	union source_id *virt_sid = &entry->virt_sid;
	union irte_index ir_index;
	union dmar_ir_entry irte;
	struct intr_source intr_src;
	int32_t ret;

	if (virt_sid->intx_id.ctlr == INTX_CTLR_IOAPIC) {
		uint64_t vdmask, pdmask;
		uint32_t dest, delmode, dest_mask, vector;
		union ioapic_rte virt_rte;
		bool phys;

		vioapic_get_rte(vm, virt_sid->intx_id.gsi, &virt_rte);
		rte = virt_rte;

		/* init polarity & pin state */
		if (rte.bits.intr_polarity == IOAPIC_RTE_INTPOL_ALO) {
			if (entry->polarity == 0U) {
				vioapic_set_irqline_nolock(vm, virt_sid->intx_id.gsi, GSI_SET_HIGH);
			}
			entry->polarity = 1U;
		} else {
			if (entry->polarity == 1U) {
				vioapic_set_irqline_nolock(vm, virt_sid->intx_id.gsi, GSI_SET_LOW);
			}
			entry->polarity = 0U;
		}

		/* physical destination cpu mask */
		phys = (virt_rte.bits.dest_mode == IOAPIC_RTE_DESTMODE_PHY);
		dest = (uint32_t)virt_rte.bits.dest_field;
		vdmask = vlapic_calc_dest_noshort(vm, false, dest, phys, false);
		pdmask = vcpumask2pcpumask(vm, vdmask);

		/* physical delivery mode */
		delmode = virt_rte.bits.delivery_mode;
		if ((delmode != IOAPIC_RTE_DELMODE_FIXED) &&
			(delmode != IOAPIC_RTE_DELMODE_LOPRI)) {
			delmode = IOAPIC_RTE_DELMODE_LOPRI;
		}

		/* update physical delivery mode, dest mode(logical) & vector */
		vector = irq_to_vector(phys_irq);
		dest_mask = calculate_logical_dest_mask(pdmask);

		irte.value.lo_64 = 0UL;
		irte.value.hi_64 = 0UL;
		irte.bits.remap.vector = vector;
		irte.bits.remap.delivery_mode = delmode;
		irte.bits.remap.dest_mode = IOAPIC_RTE_DESTMODE_LOGICAL;
		irte.bits.remap.dest = dest_mask;
		irte.bits.remap.trigger_mode = rte.bits.trigger_mode;

		intr_src.is_msi = false;
		intr_src.pid_paddr = 0UL;
		intr_src.src.ioapic_id = ioapic_irq_to_ioapic_id(phys_irq);
		ret = dmar_assign_irte(&intr_src, &irte, entry->irte_idx, &ir_index.index);

		if (ret == 0) {
			entry->irte_idx = ir_index.index;
			if (ir_index.index != INVALID_IRTE_ID) {
				rte.ir_bits.vector = vector;
				rte.ir_bits.constant = 0U;
				rte.ir_bits.intr_index_high = ir_index.bits.index_high;
				rte.ir_bits.intr_format = 1U;
				rte.ir_bits.intr_index_low = ir_index.bits.index_low;
			} else {
				rte.bits.intr_mask = 1;
			}
		} else {
			rte.bits.dest_mode = IOAPIC_RTE_DESTMODE_LOGICAL;
			rte.bits.delivery_mode = delmode;
			rte.bits.vector = vector;
			rte.bits.dest_field = dest_mask;
		}

		dev_dbg(DBG_LEVEL_IRQ, "IOAPIC RTE %s = 0x%x:%x(V) -> 0x%x:%x(P)",
			(rte.ir_bits.intr_format != 0U) ? "Remappable Format" : "Compatibility Format",
			virt_rte.u.hi_32, virt_rte.u.lo_32,
			rte.u.hi_32, rte.u.lo_32);
	} else {
		enum vpic_trigger trigger;
		union ioapic_rte phys_rte;

		/* just update trigger mode */
		ioapic_get_rte(phys_irq, &phys_rte);
		rte = phys_rte;
		rte.bits.trigger_mode = IOAPIC_RTE_TRGRMODE_EDGE;
		vpic_get_irqline_trigger_mode(vm_pic(vm), (uint32_t)virt_sid->intx_id.gsi, &trigger);
		if (trigger == LEVEL_TRIGGER) {
			rte.bits.trigger_mode = IOAPIC_RTE_TRGRMODE_LEVEL;
		}

		dev_dbg(DBG_LEVEL_IRQ, "IOAPIC RTE %s = 0x%x:%x(P) -> 0x%x:%x(P)",
			(rte.ir_bits.intr_format != 0U) ? "Remappable Format" : "Compatibility Format",
			phys_rte.u.hi_32, phys_rte.u.lo_32,
			rte.u.hi_32, rte.u.lo_32);
	}

	return rte;
}