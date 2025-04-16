void free_domheap_pages(struct page_info *pg, unsigned int order)
{
    struct domain *d = page_get_owner(pg);
    unsigned int i;
    bool_t drop_dom_ref;

    ASSERT(!in_irq());

    if ( unlikely(is_xen_heap_page(pg)) )
    {
        /* NB. May recursively lock from relinquish_memory(). */
        spin_lock_recursive(&d->page_alloc_lock);

        for ( i = 0; i < (1 << order); i++ )
            page_list_del2(&pg[i], &d->xenpage_list, &d->arch.relmem_list);

        d->xenheap_pages -= 1 << order;
        drop_dom_ref = (d->xenheap_pages == 0);

        spin_unlock_recursive(&d->page_alloc_lock);
    }
    else if ( likely(d != NULL) && likely(d != dom_cow) )
    {
        /* NB. May recursively lock from relinquish_memory(). */
        spin_lock_recursive(&d->page_alloc_lock);

        for ( i = 0; i < (1 << order); i++ )
        {
            BUG_ON((pg[i].u.inuse.type_info & PGT_count_mask) != 0);
            page_list_del2(&pg[i], &d->page_list, &d->arch.relmem_list);
        }

        drop_dom_ref = !domain_adjust_tot_pages(d, -(1 << order));

        spin_unlock_recursive(&d->page_alloc_lock);

        /*
         * Normally we expect a domain to clear pages before freeing them, if 
         * it cares about the secrecy of their contents. However, after a 
         * domain has died we assume responsibility for erasure.
         */
        if ( unlikely(d->is_dying) )
            for ( i = 0; i < (1 << order); i++ )
                scrub_one_page(&pg[i]);

        free_heap_pages(pg, order);
    }
    else if ( unlikely(d == dom_cow) )
    {
        ASSERT(order == 0); 
        scrub_one_page(pg);
        free_heap_pages(pg, 0);
        drop_dom_ref = 0;
    }
    else
    {
        /* Freeing anonymous domain-heap pages. */
        free_heap_pages(pg, order);
        drop_dom_ref = 0;
    }

    if ( drop_dom_ref )
        put_domain(d);
}