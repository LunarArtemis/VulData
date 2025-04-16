static int mov_metadata_int8_bypass_padding(MOVContext *c, AVIOContext *pb,
                                            unsigned len, const char *key)
{
    char buf[16];

    /* bypass padding bytes */
    avio_r8(pb);
    avio_r8(pb);
    avio_r8(pb);

    snprintf(buf, sizeof(buf), "%d", avio_r8(pb));
    av_dict_set(&c->fc->metadata, key, buf, 0);

    return 0;
}