static struct ptirq_remapping_info *add_msix_remapping(struct acrn_vm *vm,
	uint16_t virt_bdf, uint16_t phys_bdf, uint32_t entry_nr)
{
	struct ptirq_remapping_info *entry;
	DEFINE_MSI_SID(phys_sid, phys_bdf, entry_nr);
	DEFINE_MSI_SID(virt_sid, virt_bdf, entry_nr);

	entry = find_ptirq_entry(PTDEV_INTR_MSI, &phys_sid, NULL);
	if (entry == NULL) {
		entry = ptirq_alloc_entry(vm, PTDEV_INTR_MSI);
		if (entry != NULL) {
			entry->phys_sid.value = phys_sid.value;
			entry->virt_sid.value = virt_sid.value;
			entry->release_cb = ptirq_free_irte;

			/* update msi source and active entry */
			if (ptirq_activate_entry(entry, IRQ_INVALID) < 0) {
				ptirq_release_entry(entry);
				entry = NULL;
			}
		}

		dev_dbg(DBG_LEVEL_IRQ, "VM%d MSIX add vector mapping vbdf%x:pbdf%x idx=%d",
			vm->vm_id, virt_bdf, phys_bdf, entry_nr);
	}

	return entry;
}