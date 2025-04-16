static void
cmyk_cs_to_spotcmyk_cm(const gx_device * dev, frac c, frac m, frac y, frac k, frac out[])
{
    int * map = ((gx_devn_prn_device *) dev)->devn_params.separation_order_map;

    cmyk_cs_to_devn_cm(dev, map, c, m, y, k, out);
}