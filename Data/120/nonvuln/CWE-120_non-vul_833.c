void
gray_cs_to_devn_cm(const gx_device * dev, int * map, frac gray, frac out[])
{
    int i = dev->color_info.num_components - 1;

    for(; i >= 0; i--)                  /* Clear colors */
        out[i] = frac_0;
    if ((i = map[3]) != GX_DEVICE_COLOR_MAX_COMPONENTS)
        out[i] = frac_1 - gray;
}