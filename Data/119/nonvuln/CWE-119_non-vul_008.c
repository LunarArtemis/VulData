static int mov_metadata_int8_no_padding(MOVContext *c, AVIOContext *pb,
                                        unsigned len, const char *key)
{
    char buf[16];

    snprintf(buf, sizeof(buf), "%d", avio_r8(pb));
    av_dict_set(&c->fc->metadata, key, buf, 0);

    return 0;
}