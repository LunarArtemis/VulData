static int cond_some(tvbparse_t* tt, int offset, const tvbparse_wanted_t * wanted, tvbparse_elem_t** tok) {
    guint got_so_far = 0;
    int start = offset;
    tvbparse_elem_t* ret_tok = NULL;
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_SOME) g_warning("cond_some: START");
#endif

    if ( offset > tt->end_offset )
        return -1;

    if (++tt->recursion_depth > TVBPARSE_MAX_RECURSION_DEPTH)
        return -1;

    if ( wanted->min == 0 ) {
        ret_tok = new_tok(tt,wanted->id,offset,0,wanted);
    }

    while (got_so_far < wanted->max) {
        tvbparse_elem_t* new_elem = NULL;
        int consumed;

        if ( offset > tt->end_offset )
            return -1;

        consumed = wanted->control.subelem->condition(tt, offset, wanted->control.subelem, &new_elem);

        if(consumed >= 0) {
            if (ret_tok) {
                if (new_elem->len)
                    ret_tok->len = (new_elem->offset - ret_tok->offset) + new_elem->len;

                if (ret_tok->sub) {
                    ret_tok->sub->last->next = new_elem;
                    ret_tok->sub->last = new_elem;
                } else {
                    ret_tok->sub = new_elem;
                }
            } else {
                ret_tok = new_tok(tt, wanted->id, new_elem->offset, new_elem->len, wanted);
                ret_tok->sub = new_elem;
            }
        } else {
            break;
        }

        offset += consumed;
        got_so_far++;
    }

#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_SOME) g_warning("cond_some: got num=%u",got_so_far);
#endif

    if(got_so_far < wanted->min) {
        return -1;
    }

    *tok = ret_tok;
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_SOME) g_warning("cond_some: GOT len=%i",offset - start);
#endif
    return offset - start;
}