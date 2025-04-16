static int mov_read_ctts(MOVContext *c, AVIOContext *pb, MOVAtom atom)
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

    av_dlog(c->fc, "track[%i].ctts.entries = %i\n", c->fc->nb_streams-1, entries);

    if (!entries)
        return 0;
    if (entries >= UINT_MAX / sizeof(*sc->ctts_data))
        return AVERROR_INVALIDDATA;
    sc->ctts_data = av_malloc(entries * sizeof(*sc->ctts_data));
    if (!sc->ctts_data)
        return AVERROR(ENOMEM);
    sc->ctts_count = entries;

    for (i=0; i<entries; i++) {
        int count    =avio_rb32(pb);
        int duration =avio_rb32(pb);

        sc->ctts_data[i].count   = count;
        sc->ctts_data[i].duration= duration;

        if (FFABS(duration) > (1<<28) && i+2<entries) {
            av_log(c->fc, AV_LOG_WARNING, "CTTS invalid\n");
            av_freep(&sc->ctts_data);
            sc->ctts_count = 0;
            return 0;
        }

        if (duration < 0 && i+2<entries)
            sc->dts_shift = FFMAX(sc->dts_shift, -duration);
    }

    av_dlog(c->fc, "dts shift %d\n", sc->dts_shift);

    return 0;
}