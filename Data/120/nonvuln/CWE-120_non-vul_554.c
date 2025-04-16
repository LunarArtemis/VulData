static int vorbis_parse_setup_hdr_tdtransforms(vorbis_context *vc)
{
    GetBitContext *gb = &vc->gb;
    unsigned i, vorbis_time_count = get_bits(gb, 6) + 1;

    for (i = 0; i < vorbis_time_count; ++i) {
        unsigned vorbis_tdtransform = get_bits(gb, 16);

        av_dlog(NULL, " Vorbis time domain transform %u: %u\n",
                vorbis_time_count, vorbis_tdtransform);

        if (vorbis_tdtransform) {
            av_log(vc->avccontext, AV_LOG_ERROR, "Vorbis time domain transform data nonzero. \n");
            return AVERROR_INVALIDDATA;
        }
    }
    return 0;
}