static int create_grant_pte_mapping(
    uint64_t pte_addr, l1_pgentry_t nl1e, struct vcpu *v)
{
    int rc = GNTST_okay;
    void *va;
    unsigned long gmfn, mfn;
    struct page_info *page;
    l1_pgentry_t ol1e;
    struct domain *d = v->domain;

    adjust_guest_l1e(nl1e, d);

    gmfn = pte_addr >> PAGE_SHIFT;
    page = get_page_from_gfn(d, gmfn, NULL, P2M_ALLOC);

    if ( unlikely(!page) )
    {
        gdprintk(XENLOG_WARNING, "Could not get page for normal update\n");
        return GNTST_general_error;
    }

    mfn = page_to_mfn(page);
    va = map_domain_page(_mfn(mfn));
    va = (void *)((unsigned long)va + ((unsigned long)pte_addr & ~PAGE_MASK));

    if ( !page_lock(page) )
    {
        rc = GNTST_general_error;
        goto failed;
    }

    if ( (page->u.inuse.type_info & PGT_type_mask) != PGT_l1_page_table )
    {
        page_unlock(page);
        rc = GNTST_general_error;
        goto failed;
    }

    ol1e = *(l1_pgentry_t *)va;
    if ( !UPDATE_ENTRY(l1, (l1_pgentry_t *)va, ol1e, nl1e, mfn, v, 0) )
    {
        page_unlock(page);
        rc = GNTST_general_error;
        goto failed;
    }

    page_unlock(page);

    put_page_from_l1e(ol1e, d);

 failed:
    unmap_domain_page(va);
    put_page(page);

    return rc;
}