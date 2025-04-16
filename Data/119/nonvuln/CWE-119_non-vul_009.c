static int mov_read_chan2(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    if (atom.size < 16)
        return 0;
    avio_skip(pb, 4);
    ff_mov_read_chan(c->fc, atom.size - 4, c->fc->streams[0]->codec);
    return 0;
}