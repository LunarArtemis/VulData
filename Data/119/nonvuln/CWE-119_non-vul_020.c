static int mov_metadata_gnre(MOVContext *c, AVIOContext *pb,
                             unsigned len, const char *key)
{
    short genre;
    char buf[20];

    avio_r8(pb); // unknown

    genre = avio_r8(pb);
    if (genre < 1 || genre > ID3v1_GENRE_MAX)
        return 0;
    snprintf(buf, sizeof(buf), "%s", ff_id3v1_genre_str[genre-1]);
    av_dict_set(&c->fc->metadata, key, buf, 0);

    return 0;
}