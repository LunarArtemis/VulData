void vcpu_destroy(struct vcpu *v)
{
    xfree(v->arch.vm_event);
    v->arch.vm_event = NULL;

    vcpu_destroy_fpu(v);

    if ( !is_idle_domain(v->domain) )
        vpmu_destroy(v);

    if ( is_hvm_vcpu(v) )
        hvm_vcpu_destroy(v);
    else
        pv_vcpu_destroy(v);
}