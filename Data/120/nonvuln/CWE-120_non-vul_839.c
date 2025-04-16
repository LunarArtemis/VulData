static void
rgb_cs_to_spotcmyk_cm(const gx_device * dev, const gs_gstate *pgs,
                                   frac r, frac g, frac b, frac out[])
{
    int * map = ((gx_devn_prn_device *) dev)->devn_params.separation_order_map;

    rgb_cs_to_devn_cm(dev, map, pgs, r, g, b, out);
}