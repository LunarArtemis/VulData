static int mov_read_alac(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    return mov_read_extradata(c, pb, atom, CODEC_ID_ALAC);
}