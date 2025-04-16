static int mov_read_extradata(MOVContext *c, AVIOContext *pb, MOVAtom atom,
                              enum CodecID codec_id)
{
    AVStream *st;
    uint64_t size;
    uint8_t *buf;

    if (c->fc->nb_streams < 1) // will happen with jp2 files
        return 0;
    st= c->fc->streams[c->fc->nb_streams-1];

    if (st->codec->codec_id != codec_id)
        return 0; /* unexpected codec_id - don't mess with extradata */

    size= (uint64_t)st->codec->extradata_size + atom.size + 8 + FF_INPUT_BUFFER_PADDING_SIZE;
    if (size > INT_MAX || (uint64_t)atom.size > INT_MAX)
        return AVERROR_INVALIDDATA;
    buf= av_realloc(st->codec->extradata, size);
    if (!buf)
        return AVERROR(ENOMEM);
    st->codec->extradata= buf;
    buf+= st->codec->extradata_size;
    st->codec->extradata_size= size - FF_INPUT_BUFFER_PADDING_SIZE;
    AV_WB32(       buf    , atom.size + 8);
    AV_WL32(       buf + 4, atom.type);
    avio_read(pb, buf + 8, atom.size);
    return 0;
}