static int mov_read_moof(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    c->fragment.moof_offset = avio_tell(pb) - 8;
    av_dlog(c->fc, "moof offset %"PRIx64"\n", c->fragment.moof_offset);
    return mov_read_default(c, pb, atom);
}