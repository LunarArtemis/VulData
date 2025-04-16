static struct ptirq_remapping_info *add_intx_remapping(struct acrn_vm *vm, uint32_t virt_gsi,
		uint32_t phys_gsi, enum intx_ctlr vgsi_ctlr)
{
	struct ptirq_remapping_info *entry = NULL;
	DEFINE_INTX_SID(phys_sid, phys_gsi, INTX_CTLR_IOAPIC);
	DEFINE_INTX_SID(virt_sid, virt_gsi, vgsi_ctlr);
	uint32_t phys_irq = ioapic_gsi_to_irq(phys_gsi);

	entry = find_ptirq_entry(PTDEV_INTR_INTX, &phys_sid, NULL);
	if (entry == NULL) {
		if (find_ptirq_entry(PTDEV_INTR_INTX, &virt_sid, vm) == NULL) {
			entry = ptirq_alloc_entry(vm, PTDEV_INTR_INTX);
			if (entry != NULL) {
				entry->phys_sid.value = phys_sid.value;
				entry->virt_sid.value = virt_sid.value;
				entry->release_cb = ptirq_free_irte;

				/* activate entry */
				if (ptirq_activate_entry(entry, phys_irq) < 0) {
					ptirq_release_entry(entry);
					entry = NULL;
				}
			}
		} else {
			pr_err("INTX re-add vpin %d", virt_gsi);
		}
	} else if (entry->vm != vm) {
		if (is_sos_vm(entry->vm)) {
			entry->vm = vm;
			entry->virt_sid.value = virt_sid.value;
			entry->polarity = 0U;
		} else {
			pr_err("INTX gsi%d already in vm%d with vgsi%d, not able to add into vm%d with vgsi%d",
					phys_gsi, entry->vm->vm_id, entry->virt_sid.intx_id.gsi, vm->vm_id, virt_gsi);
			entry = NULL;
		}
	} else {
		/* The mapping has already been added to the VM. No action
		 * required.
		 */
	}


	/*
	 * ptirq entry is either created or transferred from SOS VM to Post-launched VM
	 */

	if (entry != NULL) {
		dev_dbg(DBG_LEVEL_IRQ, "VM%d INTX add pin mapping vgsi%d:pgsi%d",
			entry->vm->vm_id, virt_gsi, phys_gsi);
	}

	return entry;
}