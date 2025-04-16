static int set_msr_hyperv_pw(struct kvm_vcpu *vcpu, u32 msr, u64 data)
{
	struct kvm *kvm = vcpu->kvm;

	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
		kvm->arch.hv_guest_os_id = data;
		/* setting guest os id to zero disables hypercall page */
		if (!kvm->arch.hv_guest_os_id)
			kvm->arch.hv_hypercall &= ~HV_X64_MSR_HYPERCALL_ENABLE;
		break;
	case HV_X64_MSR_HYPERCALL: {
		u64 gfn;
		unsigned long addr;
		u8 instructions[4];

		/* if guest os id is not set hypercall should remain disabled */
		if (!kvm->arch.hv_guest_os_id)
			break;
		if (!(data & HV_X64_MSR_HYPERCALL_ENABLE)) {
			kvm->arch.hv_hypercall = data;
			break;
		}
		gfn = data >> HV_X64_MSR_HYPERCALL_PAGE_ADDRESS_SHIFT;
		addr = gfn_to_hva(kvm, gfn);
		if (kvm_is_error_hva(addr))
			return 1;
		kvm_x86_ops->patch_hypercall(vcpu, instructions);
		((unsigned char *)instructions)[3] = 0xc3; /* ret */
		if (__copy_to_user((void __user *)addr, instructions, 4))
			return 1;
		kvm->arch.hv_hypercall = data;
		mark_page_dirty(kvm, gfn);
		break;
	}
	case HV_X64_MSR_REFERENCE_TSC: {
		u64 gfn;
		HV_REFERENCE_TSC_PAGE tsc_ref;
		memset(&tsc_ref, 0, sizeof(tsc_ref));
		kvm->arch.hv_tsc_page = data;
		if (!(data & HV_X64_MSR_TSC_REFERENCE_ENABLE))
			break;
		gfn = data >> HV_X64_MSR_TSC_REFERENCE_ADDRESS_SHIFT;
		if (kvm_write_guest(kvm, data,
			&tsc_ref, sizeof(tsc_ref)))
			return 1;
		mark_page_dirty(kvm, gfn);
		break;
	}
	default:
		vcpu_unimpl(vcpu, "HYPER-V unimplemented wrmsr: 0x%x "
			    "data 0x%llx\n", msr, data);
		return 1;
	}
	return 0;
}