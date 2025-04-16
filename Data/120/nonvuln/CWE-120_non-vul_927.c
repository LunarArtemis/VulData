void ptirq_softirq(uint16_t pcpu_id)
{
	while (1) {
		struct ptirq_remapping_info *entry = ptirq_dequeue_softirq(pcpu_id);
		struct msi_info *vmsi;

		if (entry == NULL) {
			break;
		}

		vmsi = &entry->vmsi;

		/* skip any inactive entry */
		if (!is_entry_active(entry)) {
			/* service next item */
			continue;
		}

		/* handle real request */
		if (entry->intr_type == PTDEV_INTR_INTX) {
			ptirq_handle_intx(entry->vm, entry);
		} else {
			if (vmsi != NULL) {
				/* TODO: vmsi destmode check required */
				(void)vlapic_inject_msi(entry->vm, vmsi->addr.full, vmsi->data.full);
				dev_dbg(DBG_LEVEL_PTIRQ, "dev-assign: irq=0x%x MSI VR: 0x%x-0x%x",
					entry->allocated_pirq, vmsi->data.bits.vector,
					irq_to_vector(entry->allocated_pirq));
				dev_dbg(DBG_LEVEL_PTIRQ, " vmsi_addr: 0x%lx vmsi_data: 0x%x",
					vmsi->addr.full, vmsi->data.full);
			}
		}
	}
}