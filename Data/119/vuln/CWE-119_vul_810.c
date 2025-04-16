static int cond_until(tvbparse_t* tt, const int offset, const tvbparse_wanted_t * wanted, tvbparse_elem_t** tok) {
    tvbparse_elem_t* new_elem = NULL;
    int len = 0;
    int target_offset = offset;
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_UNTIL) g_warning("cond_until: START");
#endif

    if ( offset + wanted->control.until.subelem->len > tt->end_offset )
        return -1;

    do {
        len = wanted->control.until.subelem->condition(tt, target_offset++, wanted->control.until.subelem,  &new_elem);
    } while(len < 0  && target_offset+1 < tt->end_offset);

    if (len >= 0) {

        new_elem->id = wanted->id;
        new_elem->next = NULL;
        new_elem->last = NULL;
        new_elem->wanted = wanted;
        new_elem->offset = offset;

        (*tok) = new_elem;

        switch (wanted->control.until.mode) {
            case TP_UNTIL_INCLUDE:
                new_elem->len = target_offset - offset - 1 + len;
#ifdef TVBPARSE_DEBUG
                if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_UNTIL) g_warning("cond_until: GOT len=%i",target_offset - offset -1 + len);
#endif
                return target_offset - offset -1 + len;
            case TP_UNTIL_SPEND:
                new_elem->len = target_offset - offset - 1;
#ifdef TVBPARSE_DEBUG
                if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_UNTIL) g_warning("cond_until: GOT len=%i",target_offset - offset -1 + len);
#endif
                return target_offset - offset - 1 + len;
            case TP_UNTIL_LEAVE:
                new_elem->len = target_offset - offset - 1;
#ifdef TVBPARSE_DEBUG
                if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_UNTIL) g_warning("cond_until: GOT len=%i",target_offset - offset -1);
#endif
                return target_offset - offset -1;
            default:
                DISSECTOR_ASSERT_NOT_REACHED();
                return -1;
        }

    } else {
        return -1;
    }
}