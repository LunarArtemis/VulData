static int config_input(AVFilterLink *inlink)
{
    int ret;
    AVFilterContext *ctx = inlink->dst;
    FieldMatchContext *fm = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    const int w = inlink->w;
    const int h = inlink->h;

    fm->scthresh = (int64_t)((w * h * 255.0 * fm->scthresh_flt) / 100.0);

    if ((ret = av_image_alloc(fm->map_data,   fm->map_linesize,   w, h, inlink->format, 32)) < 0 ||
        (ret = av_image_alloc(fm->cmask_data, fm->cmask_linesize, w, h, inlink->format, 32)) < 0)
        return ret;

    fm->hsub = pix_desc->log2_chroma_w;
    fm->vsub = pix_desc->log2_chroma_h;

    fm->tpitchy  = FFALIGN(w,      16);
    fm->tpitchuv = FFALIGN(w >> 1, 16);

    fm->tbuffer = av_malloc(h/2 * fm->tpitchy);
    fm->c_array = av_malloc((((w + fm->blockx/2)/fm->blockx)+1) *
                            (((h + fm->blocky/2)/fm->blocky)+1) *
                            4 * sizeof(*fm->c_array));
    if (!fm->tbuffer || !fm->c_array)
        return AVERROR(ENOMEM);

    return 0;
}