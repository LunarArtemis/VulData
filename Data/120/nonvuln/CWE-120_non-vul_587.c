void kvm_vcpu_reset(struct kvm_vcpu *vcpu)
{
	atomic_set(&vcpu->arch.nmi_queued, 0);
	vcpu->arch.nmi_pending = 0;
	vcpu->arch.nmi_injected = false;

	memset(vcpu->arch.db, 0, sizeof(vcpu->arch.db));
	vcpu->arch.dr6 = DR6_FIXED_1;
	kvm_update_dr6(vcpu);
	vcpu->arch.dr7 = DR7_FIXED_1;
	kvm_update_dr7(vcpu);

	kvm_make_request(KVM_REQ_EVENT, vcpu);
	vcpu->arch.apf.msr_val = 0;
	vcpu->arch.st.msr_val = 0;

	kvmclock_reset(vcpu);

	kvm_clear_async_pf_completion_queue(vcpu);
	kvm_async_pf_hash_reset(vcpu);
	vcpu->arch.apf.halted = false;

	kvm_pmu_reset(vcpu);

	memset(vcpu->arch.regs, 0, sizeof(vcpu->arch.regs));
	vcpu->arch.regs_avail = ~0;
	vcpu->arch.regs_dirty = ~0;

	kvm_x86_ops->vcpu_reset(vcpu);
}