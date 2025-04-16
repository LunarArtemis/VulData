int
gx_devn_prn_decode_color(gx_device * dev, gx_color_index color, gx_color_value * out)
{
    int bpc = ((gx_devn_prn_device *)dev)->devn_params.bitspercomponent;
    int mask = (1 << bpc) - 1;
    int i = 0;
    uchar ncomp = dev->color_info.num_components;
    COLDUP_VARS;

    COLDUP_SETUP(bpc);
    for (; i<ncomp; i++) {
        out[ncomp - i - 1] = COLDUP_DUP(color & mask);
        color >>= bpc;
    }
    return 0;
}