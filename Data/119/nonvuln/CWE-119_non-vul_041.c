static int mov_read_trex(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVTrackExt *trex;

    if ((uint64_t)c->trex_count+1 >= UINT_MAX / sizeof(*c->trex_data))
        return AVERROR_INVALIDDATA;
    trex = av_realloc(c->trex_data, (c->trex_count+1)*sizeof(*c->trex_data));
    if (!trex)
        return AVERROR(ENOMEM);

    c->fc->duration = AV_NOPTS_VALUE; // the duration from mvhd is not representing the whole file when fragments are used.

    c->trex_data = trex;
    trex = &c->trex_data[c->trex_count++];
    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */
    trex->track_id = avio_rb32(pb);
    trex->stsd_id  = avio_rb32(pb);
    trex->duration = avio_rb32(pb);
    trex->size     = avio_rb32(pb);
    trex->flags    = avio_rb32(pb);
    return 0;
}