static int vorbis_parse_setup_hdr_mappings(vorbis_context *vc)
{
    GetBitContext *gb = &vc->gb;
    unsigned i, j;

    vc->mapping_count = get_bits(gb, 6)+1;
    vc->mappings      = av_mallocz(vc->mapping_count * sizeof(*vc->mappings));

    av_dlog(NULL, " There are %d mappings. \n", vc->mapping_count);

    for (i = 0; i < vc->mapping_count; ++i) {
        vorbis_mapping *mapping_setup = &vc->mappings[i];

        if (get_bits(gb, 16)) {
            av_log(vc->avccontext, AV_LOG_ERROR, "Other mappings than type 0 are not compliant with the Vorbis I specification. \n");
            return AVERROR_INVALIDDATA;
        }
        if (get_bits1(gb)) {
            mapping_setup->submaps = get_bits(gb, 4) + 1;
        } else {
            mapping_setup->submaps = 1;
        }

        if (get_bits1(gb)) {
            mapping_setup->coupling_steps = get_bits(gb, 8) + 1;
            mapping_setup->magnitude      = av_mallocz(mapping_setup->coupling_steps *
                                                       sizeof(*mapping_setup->magnitude));
            mapping_setup->angle          = av_mallocz(mapping_setup->coupling_steps *
                                                       sizeof(*mapping_setup->angle));
            for (j = 0; j < mapping_setup->coupling_steps; ++j) {
                GET_VALIDATED_INDEX(mapping_setup->magnitude[j], ilog(vc->audio_channels - 1), vc->audio_channels)
                GET_VALIDATED_INDEX(mapping_setup->angle[j],     ilog(vc->audio_channels - 1), vc->audio_channels)
            }
        } else {
            mapping_setup->coupling_steps = 0;
        }

        av_dlog(NULL, "   %u mapping coupling steps: %d\n",
                i, mapping_setup->coupling_steps);

        if (get_bits(gb, 2)) {
            av_log(vc->avccontext, AV_LOG_ERROR, "%u. mapping setup data invalid.\n", i);
            return AVERROR_INVALIDDATA; // following spec.
        }

        if (mapping_setup->submaps>1) {
            mapping_setup->mux = av_mallocz(vc->audio_channels *
                                            sizeof(*mapping_setup->mux));
            for (j = 0; j < vc->audio_channels; ++j)
                mapping_setup->mux[j] = get_bits(gb, 4);
        }

        for (j = 0; j < mapping_setup->submaps; ++j) {
            skip_bits(gb, 8); // FIXME check?
            GET_VALIDATED_INDEX(mapping_setup->submap_floor[j],   8, vc->floor_count)
            GET_VALIDATED_INDEX(mapping_setup->submap_residue[j], 8, vc->residue_count)

            av_dlog(NULL, "   %u mapping %u submap : floor %d, residue %d\n", i, j,
                    mapping_setup->submap_floor[j],
                    mapping_setup->submap_residue[j]);
        }
    }
    return 0;
}