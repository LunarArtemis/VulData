static int mov_read_avss(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    return mov_read_extradata(c, pb, atom, CODEC_ID_AVS);
}