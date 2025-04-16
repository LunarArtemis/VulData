static int destroy_grant_pte_mapping(
    uint64_t addr, unsigned long frame, struct domain *d)
{
    int rc = GNTST_okay;
    void *va;
    unsigned long gmfn, mfn;
    struct page_info *page;
    l1_pgentry_t ol1e;

    gmfn = addr >> PAGE_SHIFT;
    page = get_page_from_gfn(d, gmfn, NULL, P2M_ALLOC);

    if ( unlikely(!page) )
    {
        gdprintk(XENLOG_WARNING, "Could not get page for normal update\n");
        return GNTST_general_error;
    }

    mfn = page_to_mfn(page);
    va = map_domain_page(_mfn(mfn));
    va = (void *)((unsigned long)va + ((unsigned long)addr & ~PAGE_MASK));

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

    /* Check that the virtual address supplied is actually mapped to frame. */
    if ( unlikely(l1e_get_pfn(ol1e) != frame) )
    {
        page_unlock(page);
        gdprintk(XENLOG_WARNING,
                 "PTE entry %"PRIpte" for address %"PRIx64" doesn't match frame %lx\n",
                 l1e_get_intpte(ol1e), addr, frame);
        rc = GNTST_general_error;
        goto failed;
    }

    /* Delete pagetable entry. */
    if ( unlikely(!UPDATE_ENTRY(l1,
                                (l1_pgentry_t *)va, ol1e, l1e_empty(), mfn,
                                d->vcpu[0] /* Change if we go to per-vcpu shadows. */,
                                0)) )
    {
        page_unlock(page);
        gdprintk(XENLOG_WARNING, "Cannot delete PTE entry at %p\n", va);
        rc = GNTST_general_error;
        goto failed;
    }

    page_unlock(page);

 failed:
    unmap_domain_page(va);
    put_page(page);
    return rc;
}