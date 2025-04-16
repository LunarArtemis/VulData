void
rgb_cs_to_devn_cm(const gx_device * dev, int * map,
                const gs_gstate *pgs, frac r, frac g, frac b, frac out[])
{
    int i = dev->color_info.num_components - 1;
    frac cmyk[4];

    for(; i >= 0; i--)                  /* Clear colors */
        out[i] = frac_0;
    color_rgb_to_cmyk(r, g, b, pgs, cmyk, dev->memory);
    if ((i = map[0]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = cmyk[0];
    if ((i = map[1]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = cmyk[1];
    if ((i = map[2]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = cmyk[2];
    if ((i = map[3]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = cmyk[3];
}