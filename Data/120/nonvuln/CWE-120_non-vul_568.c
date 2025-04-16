static av_cold void vorbis_decode_flush(AVCodecContext *avccontext)
{
    vorbis_context *vc = avccontext->priv_data;

    if (vc->saved) {
        memset(vc->saved, 0, (vc->blocksize[1] / 4) * vc->audio_channels *
                             sizeof(*vc->saved));
    }
    vc->previous_window = 0;
}