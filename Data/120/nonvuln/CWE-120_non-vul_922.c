static void
remove_msix_remapping(const struct acrn_vm *vm, uint16_t phys_bdf, uint32_t entry_nr)
{
	struct ptirq_remapping_info *entry;
	DEFINE_MSI_SID(phys_sid, phys_bdf, entry_nr);
	struct intr_source intr_src;

	entry = find_ptirq_entry(PTDEV_INTR_MSI, &phys_sid, NULL);
	if ((entry != NULL) && (entry->vm == vm)) {
		if (is_entry_active(entry)) {
			/*TODO: disable MSIX device when HV can in future */
			ptirq_deactivate_entry(entry);
		}

		intr_src.is_msi = true;
		intr_src.src.msi.value = entry->phys_sid.msi_id.bdf;
		dmar_free_irte(&intr_src, entry->irte_idx);

		dev_dbg(DBG_LEVEL_IRQ, "VM%d MSIX remove vector mapping vbdf-pbdf:0x%x-0x%x idx=%d",
			vm->vm_id, entry->virt_sid.msi_id.bdf, phys_bdf, entry_nr);

		ptirq_release_entry(entry);
	}

}