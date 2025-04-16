static int
acquire_grant_for_copy(
    struct domain *rd, grant_ref_t gref, domid_t ldom, bool readonly,
    unsigned long *frame, struct page_info **page,
    uint16_t *page_off, uint16_t *length, bool allow_transitive)
{
    struct grant_table *rgt = rd->grant_table;
    grant_entry_v2_t *sha2;
    grant_entry_header_t *shah;
    struct active_grant_entry *act;
    grant_status_t *status;
    uint32_t old_pin;
    domid_t trans_domid;
    grant_ref_t trans_gref;
    struct domain *td;
    unsigned long grant_frame;
    uint16_t trans_page_off;
    uint16_t trans_length;
    bool is_sub_page;
    s16 rc = GNTST_okay;

    *page = NULL;

    grant_read_lock(rgt);

    if ( unlikely(gref >= nr_grant_entries(rgt)) )
        PIN_FAIL(gt_unlock_out, GNTST_bad_gntref,
                 "Bad grant reference %#x\n", gref);

    act = active_entry_acquire(rgt, gref);
    shah = shared_entry_header(rgt, gref);
    if ( rgt->gt_version == 1 )
    {
        sha2 = NULL;
        status = &shah->flags;
    }
    else
    {
        sha2 = &shared_entry_v2(rgt, gref);
        status = &status_entry(rgt, gref);
    }

    /* If already pinned, check the active domid and avoid refcnt overflow. */
    if ( act->pin && ((act->domid != ldom) || (act->pin & 0x80808080U) != 0) )
        PIN_FAIL(unlock_out, GNTST_general_error,
                 "Bad domain (%d != %d), or risk of counter overflow %08x\n",
                 act->domid, ldom, act->pin);

    old_pin = act->pin;
    if ( sha2 && (shah->flags & GTF_type_mask) == GTF_transitive )
    {
        if ( (!old_pin || (!readonly &&
                           !(old_pin & (GNTPIN_devw_mask|GNTPIN_hstw_mask)))) &&
             (rc = _set_status_v2(ldom, readonly, 0, shah, act,
                                  status)) != GNTST_okay )
            goto unlock_out;

        if ( !allow_transitive )
            PIN_FAIL(unlock_out_clear, GNTST_general_error,
                     "transitive grant when transitivity not allowed\n");

        trans_domid = sha2->transitive.trans_domid;
        trans_gref = sha2->transitive.gref;
        barrier(); /* Stop the compiler from re-loading
                      trans_domid from shared memory */
        if ( trans_domid == rd->domain_id )
            PIN_FAIL(unlock_out_clear, GNTST_general_error,
                     "transitive grants cannot be self-referential\n");

        /*
         * We allow the trans_domid == ldom case, which corresponds to a
         * grant being issued by one domain, sent to another one, and then
         * transitively granted back to the original domain.  Allowing it
         * is easy, and means that you don't need to go out of your way to
         * avoid it in the guest.
         */

        /* We need to leave the rrd locked during the grant copy. */
        td = rcu_lock_domain_by_id(trans_domid);
        if ( td == NULL )
            PIN_FAIL(unlock_out_clear, GNTST_general_error,
                     "transitive grant referenced bad domain %d\n",
                     trans_domid);

        /*
         * acquire_grant_for_copy() could take the lock on the
         * remote table (if rd == td), so we have to drop the lock
         * here and reacquire.
         */
        active_entry_release(act);
        grant_read_unlock(rgt);

        rc = acquire_grant_for_copy(td, trans_gref, rd->domain_id,
                                    readonly, &grant_frame, page,
                                    &trans_page_off, &trans_length,
                                    false);

        grant_read_lock(rgt);
        act = active_entry_acquire(rgt, gref);

        if ( rc != GNTST_okay )
        {
            fixup_status_for_copy_pin(act, status);
            rcu_unlock_domain(td);
            active_entry_release(act);
            grant_read_unlock(rgt);
            return rc;
        }

        /*
         * We dropped the lock, so we have to check that the grant didn't
         * change, and that nobody else tried to pin/unpin it. If anything
         * changed, just give up and tell the caller to retry.
         */
        if ( rgt->gt_version != 2 ||
             act->pin != old_pin ||
             (old_pin && (act->domid != ldom || act->frame != grant_frame ||
                          act->start != trans_page_off ||
                          act->length != trans_length ||
                          act->trans_domain != td ||
                          act->trans_gref != trans_gref ||
                          !act->is_sub_page)) )
        {
            release_grant_for_copy(td, trans_gref, readonly);
            fixup_status_for_copy_pin(act, status);
            rcu_unlock_domain(td);
            active_entry_release(act);
            grant_read_unlock(rgt);
            put_page(*page);
            *page = NULL;
            return ERESTART;
        }

        if ( !old_pin )
        {
            act->domid = ldom;
            act->start = trans_page_off;
            act->length = trans_length;
            act->trans_domain = td;
            act->trans_gref = trans_gref;
            act->frame = grant_frame;
            act_set_gfn(act, INVALID_GFN);
            /*
             * The actual remote remote grant may or may not be a sub-page,
             * but we always treat it as one because that blocks mappings of
             * transitive grants.
             */
            act->is_sub_page = true;
        }
    }
    else if ( !old_pin ||
              (!readonly && !(old_pin & (GNTPIN_devw_mask|GNTPIN_hstw_mask))) )
    {
        if ( (rc = _set_status(rgt->gt_version, ldom,
                               readonly, 0, shah, act,
                               status) ) != GNTST_okay )
             goto unlock_out;

        td = rd;
        trans_gref = gref;
        if ( !sha2 )
        {
            unsigned long gfn = shared_entry_v1(rgt, gref).frame;

            rc = get_paged_frame(gfn, &grant_frame, page, readonly, rd);
            if ( rc != GNTST_okay )
                goto unlock_out_clear;
            act_set_gfn(act, _gfn(gfn));
            is_sub_page = false;
            trans_page_off = 0;
            trans_length = PAGE_SIZE;
        }
        else if ( !(sha2->hdr.flags & GTF_sub_page) )
        {
            rc = get_paged_frame(sha2->full_page.frame, &grant_frame, page,
                                 readonly, rd);
            if ( rc != GNTST_okay )
                goto unlock_out_clear;
            act_set_gfn(act, _gfn(sha2->full_page.frame));
            is_sub_page = false;
            trans_page_off = 0;
            trans_length = PAGE_SIZE;
        }
        else
        {
            rc = get_paged_frame(sha2->sub_page.frame, &grant_frame, page,
                                 readonly, rd);
            if ( rc != GNTST_okay )
                goto unlock_out_clear;
            act_set_gfn(act, _gfn(sha2->sub_page.frame));
            is_sub_page = true;
            trans_page_off = sha2->sub_page.page_off;
            trans_length = sha2->sub_page.length;
        }

        if ( !act->pin )
        {
            act->domid = ldom;
            act->is_sub_page = is_sub_page;
            act->start = trans_page_off;
            act->length = trans_length;
            act->trans_domain = td;
            act->trans_gref = trans_gref;
            act->frame = grant_frame;
        }
    }
    else
    {
        ASSERT(mfn_valid(_mfn(act->frame)));
        *page = mfn_to_page(act->frame);
        td = page_get_owner_and_reference(*page);
        /*
         * act->pin being non-zero should guarantee the page to have a
         * non-zero refcount and hence a valid owner.
         */
        ASSERT(td);
    }

    act->pin += readonly ? GNTPIN_hstr_inc : GNTPIN_hstw_inc;

    *page_off = act->start;
    *length = act->length;
    *frame = act->frame;

    active_entry_release(act);
    grant_read_unlock(rgt);
    return rc;

 unlock_out_clear:
    if ( !(readonly) &&
         !(act->pin & (GNTPIN_hstw_mask | GNTPIN_devw_mask)) )
        gnttab_clear_flag(_GTF_writing, status);

    if ( !act->pin )
        gnttab_clear_flag(_GTF_reading, status);

 unlock_out:
    active_entry_release(act);

 gt_unlock_out:
    grant_read_unlock(rgt);

    return rc;
}