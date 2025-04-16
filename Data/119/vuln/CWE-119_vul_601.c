static const guint8*
get_unquoted_string(tvbuff_t *tvb, gint offset, gint *next_offset, guint *len)
{
    const guint8* s = NULL;
    guint l = 0;
    gint o;

    o = tvb_pbrk_guint8(tvb, offset, -1, " \t\r\n", NULL);
    if (o != -1) {
        l = o - offset;
        s = tvb_get_ptr(tvb, offset, l);
        offset = o;
    }

    *next_offset = offset;
    *len = l;

    return s;
}