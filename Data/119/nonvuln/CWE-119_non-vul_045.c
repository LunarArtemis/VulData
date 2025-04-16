static void mov_metadata_creation_time(AVDictionary **metadata, time_t time)
{
    char buffer[32];
    if (time) {
        struct tm *ptm;
        time -= 2082844800;  /* seconds between 1904-01-01 and Epoch */
        ptm = gmtime(&time);
        if (!ptm) return;
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ptm);
        av_dict_set(metadata, "creation_time", buffer, 0);
    }
}