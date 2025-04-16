int map_vcpu_info(struct vcpu *v, unsigned long gfn, unsigned offset)
{
    struct domain *d = v->domain;
    void *mapping;
    vcpu_info_t *new_info;
    struct page_info *page;

    if ( offset > (PAGE_SIZE - sizeof(vcpu_info_t)) )
        return -EINVAL;

    if ( !mfn_eq(v->vcpu_info_mfn, INVALID_MFN) )
        return -EINVAL;

    /* Run this command on yourself or on other offline VCPUS. */
    if ( (v != current) && !(v->pause_flags & VPF_down) )
        return -EINVAL;

    page = get_page_from_gfn(d, gfn, NULL, P2M_ALLOC);
    if ( !page )
        return -EINVAL;

    if ( !get_page_type(page, PGT_writable_page) )
    {
        put_page(page);
        return -EINVAL;
    }

    mapping = __map_domain_page_global(page);
    if ( mapping == NULL )
    {
        put_page_and_type(page);
        return -ENOMEM;
    }

    new_info = (vcpu_info_t *)(mapping + offset);

    if ( v->vcpu_info == &dummy_vcpu_info )
    {
        memset(new_info, 0, sizeof(*new_info));
#ifdef XEN_HAVE_PV_UPCALL_MASK
        __vcpu_info(v, new_info, evtchn_upcall_mask) = 1;
#endif
    }
    else
    {
        memcpy(new_info, v->vcpu_info, sizeof(*new_info));
    }

    v->vcpu_info = new_info;
    v->vcpu_info_mfn = page_to_mfn(page);

    /* Set new vcpu_info pointer /before/ setting pending flags. */
    smp_wmb();

    /*
     * Mark everything as being pending just to make sure nothing gets
     * lost.  The domain will get a spurious event, but it can cope.
     */
#ifdef CONFIG_COMPAT
    if ( !has_32bit_shinfo(d) )
        write_atomic(&new_info->native.evtchn_pending_sel, ~0);
    else
#endif
        write_atomic(&vcpu_info(v, evtchn_pending_sel), ~0);
    vcpu_mark_events_pending(v);

    return 0;
}