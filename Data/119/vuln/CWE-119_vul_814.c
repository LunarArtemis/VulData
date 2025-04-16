static int cond_one_of(tvbparse_t* tt, const int offset, const tvbparse_wanted_t * wanted, tvbparse_elem_t** tok) {
    guint i;
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_ONEOF) g_warning("cond_one_of: START");
#endif

    if ( offset > tt->end_offset )
        return -1;

    for(i=0; i < wanted->control.elems->len; i++) {
        tvbparse_wanted_t* w = (tvbparse_wanted_t *)g_ptr_array_index(wanted->control.elems,i);
        tvbparse_elem_t* new_elem = NULL;
        int curr_len;

        if ( offset + w->len > tt->end_offset )
            continue;

        curr_len = w->condition(tt, offset, w,  &new_elem);

        if (curr_len >= 0) {
            *tok = new_tok(tt, wanted->id, new_elem->offset, new_elem->len, wanted);
            (*tok)->sub = new_elem;
#ifdef TVBPARSE_DEBUG
            if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_ONEOF) g_warning("cond_one_of: GOT len=%i",curr_len);
#endif
            return curr_len;
        }
    }

    return -1;
}