void
cmyk_cs_to_devn_cm(const gx_device * dev, const int * map,
                frac c, frac m, frac y, frac k, frac out[])
{
    int i = dev->color_info.num_components - 1;

    for(; i >= 0; i--)                  /* Clear colors */
        out[i] = frac_0;
    if ((i = map[0]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = c;
    if ((i = map[1]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = m;
    if ((i = map[2]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = y;
    if ((i = map[3]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = k;
}