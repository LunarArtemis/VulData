tvbparse_t* tvbparse_init(tvbuff_t* tvb,
                          const int offset,
                          int len,
                          void* data,
                          const tvbparse_wanted_t* ignore) {
    tvbparse_t* tt = (tvbparse_t *)wmem_new(wmem_packet_scope(), tvbparse_t);

#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_TT) g_warning("tvbparse_init: offset=%i len=%i",offset,len);
#endif


    tt->tvb = tvb;
    tt->offset = offset;
    len = (len == -1) ? (int) tvb_captured_length(tvb) : len;
    tt->end_offset = offset + len;
    tt->data = data;
    tt->ignore = ignore;
    return tt;
}