static int mov_read_jp2h(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    return mov_read_extradata(c, pb, atom, CODEC_ID_JPEG2000);
}