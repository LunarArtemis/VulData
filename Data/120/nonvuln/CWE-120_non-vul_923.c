static struct acrn_vcpu *is_single_destination(struct acrn_vm *vm, const struct msi_info *info)
{
	uint64_t vdmask;
	uint16_t vid;
	struct acrn_vcpu *vcpu = NULL;

	vdmask = vlapic_calc_dest_noshort(vm, false, (uint32_t)(info->addr.bits.dest_field),
		(bool)(info->addr.bits.dest_mode == MSI_ADDR_DESTMODE_PHYS),
		(bool)(info->data.bits.delivery_mode == MSI_DATA_DELMODE_LOPRI));

	vid = ffs64(vdmask);

	/* Can only post fixed and Lowpri IRQs */
	if ((info->data.bits.delivery_mode == MSI_DATA_DELMODE_FIXED)
		|| (info->data.bits.delivery_mode == MSI_DATA_DELMODE_LOPRI)) {
		/* Can only post single-destination IRQs */
		if (vdmask == (1UL << vid)) {
			vcpu = vcpu_from_vid(vm, vid);
		}
	}

	return vcpu;
}