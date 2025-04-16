void ptirq_remove_msix_remapping(const struct acrn_vm *vm, uint16_t phys_bdf,
		uint32_t vector_count)
{
	uint32_t i;

	for (i = 0U; i < vector_count; i++) {
		spinlock_obtain(&ptdev_lock);
		remove_msix_remapping(vm, phys_bdf, i);
		spinlock_release(&ptdev_lock);
	}
}