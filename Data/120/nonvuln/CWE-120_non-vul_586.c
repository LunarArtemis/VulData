void kvm_write_tsc(struct kvm_vcpu *vcpu, struct msr_data *msr)
{
	struct kvm *kvm = vcpu->kvm;
	u64 offset, ns, elapsed;
	unsigned long flags;
	s64 usdiff;
	bool matched;
	u64 data = msr->data;

	raw_spin_lock_irqsave(&kvm->arch.tsc_write_lock, flags);
	offset = kvm_x86_ops->compute_tsc_offset(vcpu, data);
	ns = get_kernel_ns();
	elapsed = ns - kvm->arch.last_tsc_nsec;

	if (vcpu->arch.virtual_tsc_khz) {
		int faulted = 0;

		/* n.b - signed multiplication and division required */
		usdiff = data - kvm->arch.last_tsc_write;
#ifdef CONFIG_X86_64
		usdiff = (usdiff * 1000) / vcpu->arch.virtual_tsc_khz;
#else
		/* do_div() only does unsigned */
		asm("1: idivl %[divisor]\n"
		    "2: xor %%edx, %%edx\n"
		    "   movl $0, %[faulted]\n"
		    "3:\n"
		    ".section .fixup,\"ax\"\n"
		    "4: movl $1, %[faulted]\n"
		    "   jmp  3b\n"
		    ".previous\n"

		_ASM_EXTABLE(1b, 4b)

		: "=A"(usdiff), [faulted] "=r" (faulted)
		: "A"(usdiff * 1000), [divisor] "rm"(vcpu->arch.virtual_tsc_khz));

#endif
		do_div(elapsed, 1000);
		usdiff -= elapsed;
		if (usdiff < 0)
			usdiff = -usdiff;

		/* idivl overflow => difference is larger than USEC_PER_SEC */
		if (faulted)
			usdiff = USEC_PER_SEC;
	} else
		usdiff = USEC_PER_SEC; /* disable TSC match window below */

	/*
	 * Special case: TSC write with a small delta (1 second) of virtual
	 * cycle time against real time is interpreted as an attempt to
	 * synchronize the CPU.
         *
	 * For a reliable TSC, we can match TSC offsets, and for an unstable
	 * TSC, we add elapsed time in this computation.  We could let the
	 * compensation code attempt to catch up if we fall behind, but
	 * it's better to try to match offsets from the beginning.
         */
	if (usdiff < USEC_PER_SEC &&
	    vcpu->arch.virtual_tsc_khz == kvm->arch.last_tsc_khz) {
		if (!check_tsc_unstable()) {
			offset = kvm->arch.cur_tsc_offset;
			pr_debug("kvm: matched tsc offset for %llu\n", data);
		} else {
			u64 delta = nsec_to_cycles(vcpu, elapsed);
			data += delta;
			offset = kvm_x86_ops->compute_tsc_offset(vcpu, data);
			pr_debug("kvm: adjusted tsc offset by %llu\n", delta);
		}
		matched = true;
	} else {
		/*
		 * We split periods of matched TSC writes into generations.
		 * For each generation, we track the original measured
		 * nanosecond time, offset, and write, so if TSCs are in
		 * sync, we can match exact offset, and if not, we can match
		 * exact software computation in compute_guest_tsc()
		 *
		 * These values are tracked in kvm->arch.cur_xxx variables.
		 */
		kvm->arch.cur_tsc_generation++;
		kvm->arch.cur_tsc_nsec = ns;
		kvm->arch.cur_tsc_write = data;
		kvm->arch.cur_tsc_offset = offset;
		matched = false;
		pr_debug("kvm: new tsc generation %u, clock %llu\n",
			 kvm->arch.cur_tsc_generation, data);
	}

	/*
	 * We also track th most recent recorded KHZ, write and time to
	 * allow the matching interval to be extended at each write.
	 */
	kvm->arch.last_tsc_nsec = ns;
	kvm->arch.last_tsc_write = data;
	kvm->arch.last_tsc_khz = vcpu->arch.virtual_tsc_khz;

	vcpu->arch.last_guest_tsc = data;

	/* Keep track of which generation this VCPU has synchronized to */
	vcpu->arch.this_tsc_generation = kvm->arch.cur_tsc_generation;
	vcpu->arch.this_tsc_nsec = kvm->arch.cur_tsc_nsec;
	vcpu->arch.this_tsc_write = kvm->arch.cur_tsc_write;

	if (guest_cpuid_has_tsc_adjust(vcpu) && !msr->host_initiated)
		update_ia32_tsc_adjust_msr(vcpu, offset);
	kvm_x86_ops->write_tsc_offset(vcpu, offset);
	raw_spin_unlock_irqrestore(&kvm->arch.tsc_write_lock, flags);

	spin_lock(&kvm->arch.pvclock_gtod_sync_lock);
	if (matched)
		kvm->arch.nr_vcpus_matched_tsc++;
	else
		kvm->arch.nr_vcpus_matched_tsc = 0;

	kvm_track_tsc_matching(vcpu);
	spin_unlock(&kvm->arch.pvclock_gtod_sync_lock);
}