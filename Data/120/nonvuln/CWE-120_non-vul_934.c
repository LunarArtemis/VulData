void ptirq_remove_configured_intx_remappings(const struct acrn_vm *vm)
{
	const struct acrn_vm_config *vm_config = get_vm_config(vm->vm_id);
	uint32_t i;

	for (i = 0; i < vm_config->pt_intx_num; i++) {
		ptirq_remove_intx_remapping(vm, vm_config->pt_intx[i].virt_gsi, false);
	}
}