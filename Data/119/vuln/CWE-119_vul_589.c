static gint16
rlc_decode_li(enum rlc_mode mode, tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
          struct rlc_li *li, guint8 max_li, gboolean li_on_2_bytes)
{
    guint8      ext, hdr_len, offs  = 0, num_li = 0, li_offs;
    guint16     next_bytes, prev_li = 0;
    proto_item *malformed;
    guint16     total_len;

    switch (mode) {
        case RLC_AM:
            offs = 1;
            break;
        case RLC_UM:
            offs = 0;
            break;
        case RLC_TM:
            /* fall trough */
        case RLC_UNKNOWN_MODE:
        default:
            return -1;
    }
    hdr_len = offs;
    /* calculate header length */
    ext = tvb_get_guint8(tvb, hdr_len++) & 0x01;
    while (ext) {
        next_bytes = li_on_2_bytes ? tvb_get_ntohs(tvb, hdr_len) : tvb_get_guint8(tvb, hdr_len);
        ext = next_bytes & 0x01;
        hdr_len += li_on_2_bytes ? 2 : 1;
    }
    total_len = tvb_length_remaining(tvb, hdr_len);

    /* do actual evaluation of LIs */
    ext = tvb_get_guint8(tvb, offs++) & 0x01;
    li_offs = offs;
    while (ext) {
        if (li_on_2_bytes) {
            next_bytes = tvb_get_ntohs(tvb, offs);
            offs += 2;
        } else {
            next_bytes = tvb_get_guint8(tvb, offs++);
        }
        ext = next_bytes & 0x01;
        li[num_li].ext = ext;
        li[num_li].li = next_bytes >> 1;

        if (li_on_2_bytes) {
            switch (li[num_li].li) {
                case 0x0000: /* previous segment was the last one */
                case 0x7ffb: /* previous PDU contains last segment of SDU (minus last byte) */
                case 0x7ffe: /* contains piggybacked STATUS in AM or segment in UM */
                case 0x7fff: /* padding */
                    li[num_li].len = 0;
                    break;
                case 0x7ffa: /* contains exactly one SDU (minus last byte), UM only */
                case 0x7ffc: /* start of a new SDU, UM only */
                case 0x7ffd: /* contains exactly one SDU, UM only */
                    if (mode == RLC_UM) {
                        /* valid for UM */
                        li[num_li].len = 0;
                        break;
                    }
                    /*invalid for AM */
                    /* add malformed LI for investigation */
                    malformed = tree_add_li(mode, &li[num_li], num_li, li_offs, li_on_2_bytes, tvb, tree);
                    expert_add_info(pinfo, malformed, &ei_rlc_li_reserved);
                    return -1; /* just give up on this */
                default:
                    /* since the LI is an offset (from the end of the header), it
                    * may not be larger than the total remaining length and no
                    * LI may be smaller than its preceding one
                    */
                    if (((li[num_li].li > total_len) && !global_rlc_headers_expected)
                        || (li[num_li].li < prev_li)) {
                        /* add malformed LI for investigation */
                        malformed = tree_add_li(mode, &li[num_li], num_li, li_offs, li_on_2_bytes, tvb, tree);
                        expert_add_info(pinfo, malformed, &ei_rlc_li_incorrect_warn);
                        return -1; /* just give up on this */
                    }
                    li[num_li].len = li[num_li].li - prev_li;
                    prev_li = li[num_li].li;
            }
        } else {
            switch (li[num_li].li) {
                case 0x00: /* previous segment was the last one */
                case 0x7e: /* contains piggybacked STATUS in AM or segment in UM */
                case 0x7f: /* padding */
                    li[num_li].len = 0;
                    break;
                case 0x7c: /* start of a new SDU, UM only */
                case 0x7d: /* contains exactly one SDU, UM only */
                    if (mode == RLC_UM) {
                        /* valid for UM */
                        li[num_li].len = 0;
                        break;
                    }
                    /*invalid for AM */
                    /* add malformed LI for investigation */
                    malformed = tree_add_li(mode, &li[num_li], num_li, li_offs, li_on_2_bytes, tvb, tree);
                    expert_add_info(pinfo, malformed, &ei_rlc_li_reserved);
                    return -1; /* just give up on this */
                default:
                    /* since the LI is an offset (from the end of the header), it
                    * may not be larger than the total remaining length and no
                    * LI may be smaller than its preceding one
                    */
                    if (((li[num_li].li > total_len) && !global_rlc_headers_expected)
                        || (li[num_li].li < prev_li)) {
                        /* add malformed LI for investigation */
                        malformed = tree_add_li(mode, &li[num_li], num_li, li_offs, li_on_2_bytes, tvb, tree);
                        expert_add_info_format(pinfo, malformed, &ei_rlc_li_incorrect_mal, "Incorrect LI value 0x%x", li[num_li].li);
                        return -1; /* just give up on this */
                    }
                    li[num_li].len = li[num_li].li - prev_li;
                    prev_li = li[num_li].li;
            }
        }
        li[num_li].tree = tree_add_li(mode, &li[num_li], num_li, li_offs, li_on_2_bytes, tvb, tree);
        num_li++;

        if (num_li > max_li) {
            /* OK, so this is not really a malformed packet, but for now,
            * we will treat it as such, so that it is marked in some way */
            expert_add_info(pinfo, li[num_li-1].tree, &ei_rlc_li_too_many);
            return -1;
        }
    }
    return num_li;
}