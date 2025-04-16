gx_color_index
gx_devn_prn_encode_color(gx_device *dev, const gx_color_value colors[])
{
    int bpc = ((gx_devn_prn_device *)dev)->devn_params.bitspercomponent;
    gx_color_index color = 0;
    int i = 0;
    uchar ncomp = dev->color_info.num_components;
    COLROUND_VARS;

    COLROUND_SETUP(bpc);
    for (; i<ncomp; i++) {
        color <<= bpc;
        color |= COLROUND_ROUND(colors[i]);
    }
    return (color == gx_no_color_index ? color ^ 1 : color);
}