static int mov_read_chan(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    uint8_t version;
    uint32_t flags, layout_tag, bitmap, num_descr, label_mask;
    int i;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if (atom.size < 16)
        return 0;

    version = avio_r8(pb);
    flags   = avio_rb24(pb);

    layout_tag = avio_rb32(pb);
    bitmap     = avio_rb32(pb);
    num_descr  = avio_rb32(pb);

    if (atom.size < 16ULL + num_descr * 20ULL)
        return 0;

    av_dlog(c->fc, "chan: size=%" PRId64 " version=%u flags=%u layout=%u bitmap=%u num_descr=%u\n",
            atom.size, version, flags, layout_tag, bitmap, num_descr);

    label_mask = 0;
    for (i = 0; i < num_descr; i++) {
        uint32_t av_unused label, cflags;
        label     = avio_rb32(pb);          // mChannelLabel
        cflags    = avio_rb32(pb);          // mChannelFlags
        avio_rl32(pb);                      // mCoordinates[0]
        avio_rl32(pb);                      // mCoordinates[1]
        avio_rl32(pb);                      // mCoordinates[2]
        if (layout_tag == 0) {
            uint32_t mask_incr = ff_mov_get_channel_label(label);
            if (mask_incr == 0) {
                label_mask = 0;
                break;
            }
            label_mask |= mask_incr;
        }
    }
    if (layout_tag == 0)
        st->codec->channel_layout = label_mask;
    else
        st->codec->channel_layout = ff_mov_get_channel_layout(layout_tag, bitmap);

    return 0;
}