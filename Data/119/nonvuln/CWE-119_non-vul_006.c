static int mov_read_header(AVFormatContext *s)
{
    MOVContext *mov = s->priv_data;
    AVIOContext *pb = s->pb;
    int err;
    MOVAtom atom = { AV_RL32("root") };

    mov->fc = s;
    /* .mov and .mp4 aren't streamable anyway (only progressive download if moov is before mdat) */
    if (pb->seekable)
        atom.size = avio_size(pb);
    else
        atom.size = INT64_MAX;

    /* check MOV header */
    if ((err = mov_read_default(mov, pb, atom)) < 0) {
        av_log(s, AV_LOG_ERROR, "error reading header: %d\n", err);
        mov_read_close(s);
        return err;
    }
    if (!mov->found_moov) {
        av_log(s, AV_LOG_ERROR, "moov atom not found\n");
        mov_read_close(s);
        return AVERROR_INVALIDDATA;
    }
    av_dlog(mov->fc, "on_parse_exit_offset=%"PRId64"\n", avio_tell(pb));

    if (pb->seekable) {
        int i;
        if (mov->chapter_track > 0)
            mov_read_chapters(s);
        for (i = 0; i < s->nb_streams; i++)
            if (s->streams[i]->codec->codec_tag == AV_RL32("tmcd"))
                mov_read_timecode_track(s, s->streams[i]);
    }

    if (mov->trex_data) {
        int i;
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            MOVStreamContext *sc = st->priv_data;
            if (st->duration)
                st->codec->bit_rate = sc->data_size * 8 * sc->time_scale / st->duration;
        }
    }

    return 0;
}