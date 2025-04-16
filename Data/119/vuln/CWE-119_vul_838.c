int shadow_track_dirty_vram(struct domain *d,
                            unsigned long begin_pfn,
                            unsigned long nr,
                            XEN_GUEST_HANDLE(void) guest_dirty_bitmap)
{
    int rc = 0;
    unsigned long end_pfn = begin_pfn + nr;
    unsigned long dirty_size = (nr + 7) / 8;
    int flush_tlb = 0;
    unsigned long i;
    p2m_type_t t;
    struct sh_dirty_vram *dirty_vram;
    struct p2m_domain *p2m = p2m_get_hostp2m(d);
    uint8_t *dirty_bitmap = NULL;

    if ( end_pfn < begin_pfn || end_pfn > p2m->max_mapped_pfn + 1 )
        return -EINVAL;

    /* We perform p2m lookups, so lock the p2m upfront to avoid deadlock */
    p2m_lock(p2m_get_hostp2m(d));
    paging_lock(d);

    dirty_vram = d->arch.hvm.dirty_vram;

    if ( dirty_vram && (!nr ||
             ( begin_pfn != dirty_vram->begin_pfn
            || end_pfn   != dirty_vram->end_pfn )) )
    {
        /* Different tracking, tear the previous down. */
        gdprintk(XENLOG_INFO, "stopping tracking VRAM %lx - %lx\n", dirty_vram->begin_pfn, dirty_vram->end_pfn);
        xfree(dirty_vram->sl1ma);
        xfree(dirty_vram->dirty_bitmap);
        xfree(dirty_vram);
        dirty_vram = d->arch.hvm.dirty_vram = NULL;
    }

    if ( !nr )
        goto out;

    dirty_bitmap = vzalloc(dirty_size);
    if ( dirty_bitmap == NULL )
    {
        rc = -ENOMEM;
        goto out;
    }
    /* This should happen seldomly (Video mode change),
     * no need to be careful. */
    if ( !dirty_vram )
    {
        /* Throw away all the shadows rather than walking through them
         * up to nr times getting rid of mappings of each pfn */
        shadow_blow_tables(d);

        gdprintk(XENLOG_INFO, "tracking VRAM %lx - %lx\n", begin_pfn, end_pfn);

        rc = -ENOMEM;
        if ( (dirty_vram = xmalloc(struct sh_dirty_vram)) == NULL )
            goto out;
        dirty_vram->begin_pfn = begin_pfn;
        dirty_vram->end_pfn = end_pfn;
        d->arch.hvm.dirty_vram = dirty_vram;

        if ( (dirty_vram->sl1ma = xmalloc_array(paddr_t, nr)) == NULL )
            goto out_dirty_vram;
        memset(dirty_vram->sl1ma, ~0, sizeof(paddr_t) * nr);

        if ( (dirty_vram->dirty_bitmap = xzalloc_array(uint8_t, dirty_size)) == NULL )
            goto out_sl1ma;

        dirty_vram->last_dirty = NOW();

        /* Tell the caller that this time we could not track dirty bits. */
        rc = -ENODATA;
    }
    else if (dirty_vram->last_dirty == -1)
        /* still completely clean, just copy our empty bitmap */
        memcpy(dirty_bitmap, dirty_vram->dirty_bitmap, dirty_size);
    else
    {
        mfn_t map_mfn = INVALID_MFN;
        void *map_sl1p = NULL;

        /* Iterate over VRAM to track dirty bits. */
        for ( i = 0; i < nr; i++ ) {
            mfn_t mfn = get_gfn_query_unlocked(d, begin_pfn + i, &t);
            struct page_info *page;
            int dirty = 0;
            paddr_t sl1ma = dirty_vram->sl1ma[i];

            if ( !mfn_eq(mfn, INVALID_MFN) )
            {
                dirty = 1;
            }
            else
            {
                page = mfn_to_page(mfn);
                switch (page->u.inuse.type_info & PGT_count_mask)
                {
                case 0:
                    /* No guest reference, nothing to track. */
                    break;
                case 1:
                    /* One guest reference. */
                    if ( sl1ma == INVALID_PADDR )
                    {
                        /* We don't know which sl1e points to this, too bad. */
                        dirty = 1;
                        /* TODO: Heuristics for finding the single mapping of
                         * this gmfn */
                        flush_tlb |= sh_remove_all_mappings(d, mfn,
                                                            _gfn(begin_pfn + i));
                    }
                    else
                    {
                        /* Hopefully the most common case: only one mapping,
                         * whose dirty bit we can use. */
                        l1_pgentry_t *sl1e;
                        mfn_t sl1mfn = maddr_to_mfn(sl1ma);

                        if ( !mfn_eq(sl1mfn, map_mfn) )
                        {
                            if ( map_sl1p )
                                unmap_domain_page(map_sl1p);
                            map_sl1p = map_domain_page(sl1mfn);
                            map_mfn = sl1mfn;
                        }
                        sl1e = map_sl1p + (sl1ma & ~PAGE_MASK);

                        if ( l1e_get_flags(*sl1e) & _PAGE_DIRTY )
                        {
                            dirty = 1;
                            /* Note: this is atomic, so we may clear a
                             * _PAGE_ACCESSED set by another processor. */
                            l1e_remove_flags(*sl1e, _PAGE_DIRTY);
                            flush_tlb = 1;
                        }
                    }
                    break;
                default:
                    /* More than one guest reference,
                     * we don't afford tracking that. */
                    dirty = 1;
                    break;
                }
            }

            if ( dirty )
            {
                dirty_vram->dirty_bitmap[i / 8] |= 1 << (i % 8);
                dirty_vram->last_dirty = NOW();
            }
        }

        if ( map_sl1p )
            unmap_domain_page(map_sl1p);

        memcpy(dirty_bitmap, dirty_vram->dirty_bitmap, dirty_size);
        memset(dirty_vram->dirty_bitmap, 0, dirty_size);
        if ( dirty_vram->last_dirty + SECONDS(2) < NOW() )
        {
            /* was clean for more than two seconds, try to disable guest
             * write access */
            for ( i = begin_pfn; i < end_pfn; i++ )
            {
                mfn_t mfn = get_gfn_query_unlocked(d, i, &t);
                if ( !mfn_eq(mfn, INVALID_MFN) )
                    flush_tlb |= sh_remove_write_access(d, mfn, 1, 0);
            }
            dirty_vram->last_dirty = -1;
        }
    }
    if ( flush_tlb )
        guest_flush_tlb_mask(d, d->dirty_cpumask);
    goto out;

out_sl1ma:
    xfree(dirty_vram->sl1ma);
out_dirty_vram:
    xfree(dirty_vram);
    dirty_vram = d->arch.hvm.dirty_vram = NULL;

out:
    paging_unlock(d);
    if ( rc == 0 && dirty_bitmap != NULL &&
         copy_to_guest(guest_dirty_bitmap, dirty_bitmap, dirty_size) )
    {
        paging_lock(d);
        for ( i = 0; i < dirty_size; i++ )
            dirty_vram->dirty_bitmap[i] |= dirty_bitmap[i];
        paging_unlock(d);
        rc = -EFAULT;
    }
    vfree(dirty_bitmap);
    p2m_unlock(p2m_get_hostp2m(d));
    return rc;
}