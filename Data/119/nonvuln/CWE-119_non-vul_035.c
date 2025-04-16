static int mov_read_elst(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;
    int i, edit_count, version, edit_start_index = 0;

    if (c->fc->nb_streams < 1)
        return 0;
    sc = c->fc->streams[c->fc->nb_streams-1]->priv_data;

    version = avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */
    edit_count = avio_rb32(pb); /* entries */

    if ((uint64_t)edit_count*12+8 > atom.size)
        return AVERROR_INVALIDDATA;

    for (i=0; i<edit_count; i++){
        int64_t time;
        int64_t duration;
        if (version == 1) {
            duration = avio_rb64(pb);
            time     = avio_rb64(pb);
        } else {
            duration = avio_rb32(pb); /* segment duration */
            time     = (int32_t)avio_rb32(pb); /* media time */
        }
        avio_rb32(pb); /* Media rate */
        if (i == 0 && time == -1) {
            sc->empty_duration = duration;
            edit_start_index = 1;
        } else if (i == edit_start_index && time >= 0)
            sc->start_time = time;
    }

    if (edit_count > 1)
        av_log(c->fc, AV_LOG_WARNING, "multiple edit list entries, "
               "a/v desync might occur, patch welcome\n");

    av_dlog(c->fc, "track[%i].edit_count = %i\n", c->fc->nb_streams-1, edit_count);
    return 0;
}