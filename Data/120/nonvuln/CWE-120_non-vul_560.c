static void vorbis_free(vorbis_context *vc)
{
    int i;

    av_freep(&vc->channel_residues);
    av_freep(&vc->saved);

    for (i = 0; i < vc->residue_count; i++)
        av_free(vc->residues[i].classifs);
    av_freep(&vc->residues);
    av_freep(&vc->modes);

    ff_mdct_end(&vc->mdct[0]);
    ff_mdct_end(&vc->mdct[1]);

    for (i = 0; i < vc->codebook_count; ++i) {
        av_free(vc->codebooks[i].codevectors);
        ff_free_vlc(&vc->codebooks[i].vlc);
    }
    av_freep(&vc->codebooks);

    for (i = 0; i < vc->floor_count; ++i) {
        if (vc->floors[i].floor_type == 0) {
            av_free(vc->floors[i].data.t0.map[0]);
            av_free(vc->floors[i].data.t0.map[1]);
            av_free(vc->floors[i].data.t0.book_list);
            av_free(vc->floors[i].data.t0.lsp);
        } else {
            av_free(vc->floors[i].data.t1.list);
        }
    }
    av_freep(&vc->floors);

    for (i = 0; i < vc->mapping_count; ++i) {
        av_free(vc->mappings[i].magnitude);
        av_free(vc->mappings[i].angle);
        av_free(vc->mappings[i].mux);
    }
    av_freep(&vc->mappings);
}