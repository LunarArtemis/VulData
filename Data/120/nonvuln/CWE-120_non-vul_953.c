static void dmar_setup_interrupt(struct dmar_drhd_rt *dmar_unit)
{
	uint32_t vector;
	int32_t retval = 0;

	spinlock_obtain(&(dmar_unit->lock));
	if (dmar_unit->dmar_irq == IRQ_INVALID) {
		retval = request_irq(IRQ_INVALID, dmar_fault_handler, dmar_unit, IRQF_NONE);
		dmar_unit->dmar_irq = (uint32_t)retval;
	}
	spinlock_release(&(dmar_unit->lock));
	/* the panic will only happen before any VM starts running */
	if (retval < 0) {
		panic("dmar[%d] fail to setup interrupt", dmar_unit->index);
	}

	vector = irq_to_vector(dmar_unit->dmar_irq);
	dev_dbg(DBG_LEVEL_IOMMU, "irq#%d vector#%d for dmar_unit", dmar_unit->dmar_irq, vector);

	dmar_fault_msi_write(dmar_unit, vector);
	dmar_fault_event_unmask(dmar_unit);
}