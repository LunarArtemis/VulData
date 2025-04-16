static inline int sh_get_ref(struct domain *d, mfn_t smfn, paddr_t entry_pa)
{
    u32 x, nx;
    struct page_info *sp = mfn_to_page(smfn);

    ASSERT(mfn_valid(smfn));
    ASSERT(sp->u.sh.head);

    x = sp->u.sh.count;
    nx = x + 1;

    if ( unlikely(nx >= 1U<<26) )
    {
        SHADOW_PRINTK("shadow ref overflow, gmfn=%lx smfn=%lx\n",
                       __backpointer(sp), mfn_x(smfn));
        return 0;
    }

    /* Guarded by the paging lock, so no need for atomic update */
    sp->u.sh.count = nx;

    /* We remember the first shadow entry that points to each shadow. */
    if ( entry_pa != 0
         && sh_type_has_up_pointer(d, sp->u.sh.type)
         && sp->up == 0 )
        sp->up = entry_pa;

    return 1;
}