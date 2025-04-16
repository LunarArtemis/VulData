void ptirq_remove_intx_remapping(const struct acrn_vm *vm, uint32_t virt_gsi, bool pic_pin)
{
	enum intx_ctlr vgsi_ctlr = pic_pin ? INTX_CTLR_PIC : INTX_CTLR_IOAPIC;

	spinlock_obtain(&ptdev_lock);
	remove_intx_remapping(vm, virt_gsi, vgsi_ctlr);
	spinlock_release(&ptdev_lock);
}