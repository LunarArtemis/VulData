int32_t ptirq_intx_pin_remap(struct acrn_vm *vm, uint32_t virt_gsi, enum intx_ctlr vgsi_ctlr)
{
	int32_t status = 0;
	struct ptirq_remapping_info *entry = NULL;
	DEFINE_INTX_SID(virt_sid, virt_gsi, vgsi_ctlr);
	DEFINE_INTX_SID(alt_virt_sid, virt_gsi, vgsi_ctlr);

	/*
	 * virt pin could come from primary vPIC, secondary vPIC or vIOAPIC
	 * while phys pin is always means for physical IOAPIC.
	 *
	 * Device Model should pre-hold the mapping entries by calling
	 * ptirq_add_intx_remapping for UOS.
	 *
	 * For SOS(sos_vm), it adds the mapping entries at runtime, if the
	 * entry already be held by others, return error.
	 */

	/* no remap for vuart intx */
	if (!is_vuart_intx(vm, virt_sid.intx_id.gsi)) {
		/* query if we have virt to phys mapping */
		spinlock_obtain(&ptdev_lock);
		entry = find_ptirq_entry(PTDEV_INTR_INTX, &virt_sid, vm);
		if (entry == NULL) {
			if (is_sos_vm(vm)) {

				/* for sos_vm, there is chance of vpin source switch
				 * between vPIC & vIOAPIC for one legacy phys_pin.
				 *
				 * here checks if there is already mapping entry from
				 * the other vpin source for legacy pin. If yes, then
				 * switch vpin source is needed
				 */
				if (virt_gsi < NR_LEGACY_PIN) {

					if (vgsi_ctlr == INTX_CTLR_PIC) {
						alt_virt_sid.intx_id.ctlr = INTX_CTLR_IOAPIC;
					} else {
						alt_virt_sid.intx_id.ctlr = INTX_CTLR_PIC;
					}

					entry = find_ptirq_entry(PTDEV_INTR_INTX, &alt_virt_sid, vm);
					if (entry != NULL) {
						uint32_t phys_gsi = virt_gsi;

						remove_intx_remapping(vm, alt_virt_sid.intx_id.gsi,
							alt_virt_sid.intx_id.ctlr);
						entry = add_intx_remapping(vm, virt_gsi, phys_gsi, vgsi_ctlr);
						if (entry == NULL) {
							pr_err("%s, add intx remapping failed", __func__);
							status = -ENODEV;
						} else {
							dev_dbg(DBG_LEVEL_IRQ,
								"IOAPIC gsi=%hhu pirq=%u vgsi=%d from %s to %s for vm%d",
								entry->phys_sid.intx_id.gsi,
								entry->allocated_pirq, entry->virt_sid.intx_id.gsi,
								(vgsi_ctlr == INTX_CTLR_IOAPIC) ? "vPIC" : "vIOAPIC",
								(vgsi_ctlr == INTX_CTLR_IOAPIC) ? "vIOPIC" : "vPIC",
								entry->vm->vm_id);
						}
					}
				}

				/* entry could be updated by above switch check */
				if (entry == NULL) {
					uint32_t phys_gsi = virt_gsi;

					entry = add_intx_remapping(vm, virt_gsi, phys_gsi, vgsi_ctlr);
					if (entry == NULL) {
						pr_err("%s, add intx remapping failed",
								__func__);
						status = -ENODEV;
					}
				}
			} else {
				/* ptirq_intx_pin_remap is triggered by vPIC/vIOAPIC
				 * everytime a pin get unmask, here filter out pins
				 * not get mapped.
				 */
				status = -ENODEV;
			}
		}
		spinlock_release(&ptdev_lock);
	} else {
		status = -EINVAL;
	}

	if (status == 0) {
		activate_physical_ioapic(vm, entry);
	}

	return status;
}