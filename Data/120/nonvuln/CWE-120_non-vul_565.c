static int vorbis_parse_setup_hdr_modes(vorbis_context *vc)
{
    GetBitContext *gb = &vc->gb;
    unsigned i;

    vc->mode_count = get_bits(gb, 6) + 1;
    vc->modes      = av_mallocz(vc->mode_count * sizeof(*vc->modes));

    av_dlog(NULL, " There are %d modes.\n", vc->mode_count);

    for (i = 0; i < vc->mode_count; ++i) {
        vorbis_mode *mode_setup = &vc->modes[i];

        mode_setup->blockflag     = get_bits1(gb);
        mode_setup->windowtype    = get_bits(gb, 16); //FIXME check
        mode_setup->transformtype = get_bits(gb, 16); //FIXME check
        GET_VALIDATED_INDEX(mode_setup->mapping, 8, vc->mapping_count);

        av_dlog(NULL, " %u mode: blockflag %d, windowtype %d, transformtype %d, mapping %d\n",
                i, mode_setup->blockflag, mode_setup->windowtype,
                mode_setup->transformtype, mode_setup->mapping);
    }
    return 0;
}