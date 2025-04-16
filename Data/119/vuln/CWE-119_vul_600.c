static const guint8*
get_quoted_string(tvbuff_t *tvb, gint offset, gint *next_offset, guint *len)
{
    int c;
    const guint8* s = NULL;
    guint l = 0;
    gint o;

    c = tvb_get_guint8(tvb, offset);
    if (c == '"') {
        o = tvb_find_guint8(tvb, offset+1, -1, '"');
        if (o != -1) {
            offset++;
            l = o - offset;
            s = tvb_get_ptr(tvb, offset, l);
            offset = o + 1;
        }
    }

    *next_offset = offset;
    *len = l;

    return s;
}