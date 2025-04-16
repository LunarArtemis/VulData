static int mov_read_stss(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */

    entries = avio_rb32(pb);

    av_dlog(c->fc, "keyframe_count = %d\n", entries);

    if (!entries)
    {
        sc->keyframe_absent = 1;
        return 0;
    }
    if (entries >= UINT_MAX / sizeof(int))
        return AVERROR_INVALIDDATA;
    sc->keyframes = av_malloc(entries * sizeof(int));
    if (!sc->keyframes)
        return AVERROR(ENOMEM);
    sc->keyframe_count = entries;

    for (i=0; i<entries; i++) {
        sc->keyframes[i] = avio_rb32(pb);
        //av_dlog(c->fc, "keyframes[]=%d\n", sc->keyframes[i]);
    }
    return 0;
}