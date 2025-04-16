static av_cold int vorbis_decode_close(AVCodecContext *avccontext)
{
    vorbis_context *vc = avccontext->priv_data;

    vorbis_free(vc);

    return 0;
}