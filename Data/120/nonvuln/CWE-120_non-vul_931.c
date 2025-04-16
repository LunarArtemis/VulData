int32_t ptirq_add_intx_remapping(struct acrn_vm *vm, uint32_t virt_gsi, uint32_t phys_gsi, bool pic_pin)
{
	struct ptirq_remapping_info *entry;
	enum intx_ctlr vgsi_ctlr = pic_pin ? INTX_CTLR_PIC : INTX_CTLR_IOAPIC;

	spinlock_obtain(&ptdev_lock);
	entry = add_intx_remapping(vm, virt_gsi, phys_gsi, vgsi_ctlr);
	spinlock_release(&ptdev_lock);

	return (entry != NULL) ? 0 : -ENODEV;
}