static int mov_read_trun(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVFragment *frag = &c->fragment;
    AVStream *st = NULL;
    MOVStreamContext *sc;
    MOVStts *ctts_data;
    uint64_t offset;
    int64_t dts;
    int data_offset = 0;
    unsigned entries, first_sample_flags = frag->flags;
    int flags, distance, i, found_keyframe = 0;

    for (i = 0; i < c->fc->nb_streams; i++) {
        if (c->fc->streams[i]->id == frag->track_id) {
            st = c->fc->streams[i];
            break;
        }
    }
    if (!st) {
        av_log(c->fc, AV_LOG_ERROR, "could not find corresponding track id %d\n", frag->track_id);
        return AVERROR_INVALIDDATA;
    }
    sc = st->priv_data;
    if (sc->pseudo_stream_id+1 != frag->stsd_id)
        return 0;
    avio_r8(pb); /* version */
    flags = avio_rb24(pb);
    entries = avio_rb32(pb);
    av_dlog(c->fc, "flags 0x%x entries %d\n", flags, entries);

    /* Always assume the presence of composition time offsets.
     * Without this assumption, for instance, we cannot deal with a track in fragmented movies that meet the following.
     *  1) in the initial movie, there are no samples.
     *  2) in the first movie fragment, there is only one sample without composition time offset.
     *  3) in the subsequent movie fragments, there are samples with composition time offset. */
    if (!sc->ctts_count && sc->sample_count)
    {
        /* Complement ctts table if moov atom doesn't have ctts atom. */
        ctts_data = av_malloc(sizeof(*sc->ctts_data));
        if (!ctts_data)
            return AVERROR(ENOMEM);
        sc->ctts_data = ctts_data;
        sc->ctts_data[sc->ctts_count].count = sc->sample_count;
        sc->ctts_data[sc->ctts_count].duration = 0;
        sc->ctts_count++;
    }
    if ((uint64_t)entries+sc->ctts_count >= UINT_MAX/sizeof(*sc->ctts_data))
        return AVERROR_INVALIDDATA;
    ctts_data = av_realloc(sc->ctts_data,
                           (entries+sc->ctts_count)*sizeof(*sc->ctts_data));
    if (!ctts_data)
        return AVERROR(ENOMEM);
    sc->ctts_data = ctts_data;

    if (flags & MOV_TRUN_DATA_OFFSET)        data_offset        = avio_rb32(pb);
    if (flags & MOV_TRUN_FIRST_SAMPLE_FLAGS) first_sample_flags = avio_rb32(pb);
    dts    = sc->track_end - sc->time_offset;
    offset = frag->base_data_offset + data_offset;
    distance = 0;
    av_dlog(c->fc, "first sample flags 0x%x\n", first_sample_flags);
    for (i = 0; i < entries; i++) {
        unsigned sample_size = frag->size;
        int sample_flags = i ? frag->flags : first_sample_flags;
        unsigned sample_duration = frag->duration;
        int keyframe = 0;

        if (flags & MOV_TRUN_SAMPLE_DURATION) sample_duration = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_SIZE)     sample_size     = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_FLAGS)    sample_flags    = avio_rb32(pb);
        sc->ctts_data[sc->ctts_count].count = 1;
        sc->ctts_data[sc->ctts_count].duration = (flags & MOV_TRUN_SAMPLE_CTS) ?
                                                  avio_rb32(pb) : 0;
        sc->ctts_count++;
        if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO)
            keyframe = 1;
        else if (!found_keyframe)
            keyframe = found_keyframe =
                !(sample_flags & (MOV_FRAG_SAMPLE_FLAG_IS_NON_SYNC |
                                  MOV_FRAG_SAMPLE_FLAG_DEPENDS_YES));
        if (keyframe)
            distance = 0;
        av_add_index_entry(st, offset, dts, sample_size, distance,
                           keyframe ? AVINDEX_KEYFRAME : 0);
        av_dlog(c->fc, "AVIndex stream %d, sample %d, offset %"PRIx64", dts %"PRId64", "
                "size %d, distance %d, keyframe %d\n", st->index, sc->sample_count+i,
                offset, dts, sample_size, distance, keyframe);
        distance++;
        dts += sample_duration;
        offset += sample_size;
        sc->data_size += sample_size;
    }
    frag->moof_offset = offset;
    st->duration = sc->track_end = dts + sc->time_offset;
    return 0;
}