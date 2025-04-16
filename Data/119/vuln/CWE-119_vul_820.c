static int cond_hash(tvbparse_t* tt, const int offset, const tvbparse_wanted_t* wanted, tvbparse_elem_t** tok) {
    int key_len;
    gchar* key = NULL;
    tvbparse_elem_t* key_elem = NULL;
    tvbparse_wanted_t* value_wanted = NULL;
    int value_len;
    tvbparse_elem_t* value_elem = NULL;
    int tot_len;
    tvbparse_elem_t* ret_tok;

#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_HASH) g_warning("cond_hash: START");
#endif

    if ( offset > tt->end_offset )
        return -1;

    if (++tt->recursion_depth > TVBPARSE_MAX_RECURSION_DEPTH)
        return -1;

    key_len = wanted->control.hash.key->condition(tt, offset, wanted->control.hash.key,  &key_elem);

    if (key_len < 0)
        return -1;

    key = tvb_get_string_enc(wmem_packet_scope(),key_elem->tvb,key_elem->offset,key_elem->len, ENC_ASCII);
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_HASH) g_warning("cond_hash: got key='%s'",key);
#endif

    if ((value_wanted = (tvbparse_wanted_t *)wmem_map_lookup(wanted->control.hash.table,key))) {
        value_len = value_wanted->condition(tt, offset + key_len, value_wanted,  &value_elem);
    } else if (wanted->control.hash.other) {
        value_len = wanted->control.hash.other->condition(tt, offset+key_len, wanted->control.hash.other,  &value_elem);
        if (value_len < 0)
            return -1;
    } else {
        return -1;
    }

    tot_len = key_len + value_len;

    ret_tok = new_tok(tt, value_elem->id, offset, tot_len, wanted);
    ret_tok->sub = key_elem;
    ret_tok->sub->last->next = value_elem;

    *tok = ret_tok;
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_HASH) g_warning("cond_hash: GOT len=%i",tot_len);
#endif

    return tot_len;
}