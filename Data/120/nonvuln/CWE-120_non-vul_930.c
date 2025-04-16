int32_t ptirq_prepare_msix_remap(struct acrn_vm *vm, uint16_t virt_bdf, uint16_t phys_bdf,
				uint16_t entry_nr, struct msi_info *info, uint16_t irte_idx)
{
	struct ptirq_remapping_info *entry;
	int32_t ret = -ENODEV;
	union pci_bdf vbdf;

	/*
	 * adds the mapping entries at runtime, if the
	 * entry already be held by others, return error.
	 */
	spinlock_obtain(&ptdev_lock);
	entry = add_msix_remapping(vm, virt_bdf, phys_bdf, entry_nr);
	spinlock_release(&ptdev_lock);

	if (entry != NULL) {
		ret = 0;
		entry->vmsi = *info;

		/* build physical config MSI, update to info->pmsi_xxx */
		if (is_lapic_pt_configured(vm)) {
			enum vm_vlapic_mode vlapic_mode = check_vm_vlapic_mode(vm);

			if (vlapic_mode == VM_VLAPIC_X2APIC) {
				/*
				 * All the vCPUs are in x2APIC mode and LAPIC is Pass-through
				 * Use guest vector to program the interrupt source
				 */
				ptirq_build_physical_msi(vm, entry, (uint32_t)info->data.bits.vector, 0UL, irte_idx);
			} else if (vlapic_mode == VM_VLAPIC_XAPIC) {
				/*
				 * All the vCPUs are in xAPIC mode and LAPIC is emulated
				 * Use host vector to program the interrupt source
				 */
				ptirq_build_physical_msi(vm, entry, irq_to_vector(entry->allocated_pirq), 0UL, irte_idx);
			} else if (vlapic_mode == VM_VLAPIC_TRANSITION) {
				/*
				 * vCPUs are in middle of transition, so do not program interrupt source
				 * TODO: Devices programmed during transistion do not work after transition
				 * as device is not programmed with interrupt info. Need to implement a
				 * method to get interrupts working after transition.
				 */
				ret = -EFAULT;
			} else {
				/* Do nothing for VM_VLAPIC_DISABLED */
				ret = -EFAULT;
			}
		} else {
			struct acrn_vcpu *vcpu = is_single_destination(vm, info);

			if (is_pi_capable(vm) && (vcpu != NULL)) {
				ptirq_build_physical_msi(vm, entry,
					(uint32_t)info->data.bits.vector, hva2hpa(get_pi_desc(vcpu)), irte_idx);
			} else {
				/* Go with remapped mode if we cannot handle it in posted mode */
				ptirq_build_physical_msi(vm, entry, irq_to_vector(entry->allocated_pirq), 0UL, irte_idx);
			}
		}

		if (ret == 0) {
			*info = entry->pmsi;
			vbdf.value = virt_bdf;
			dev_dbg(DBG_LEVEL_IRQ, "PCI %x:%x.%x MSI VR[%d] 0x%x->0x%x assigned to vm%d",
				vbdf.bits.b, vbdf.bits.d, vbdf.bits.f, entry_nr, entry->vmsi.data.bits.vector,
				irq_to_vector(entry->allocated_pirq), entry->vm->vm_id);
		}
	}

	return ret;
}