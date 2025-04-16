static void
gray_cs_to_spotcmyk_cm(const gx_device * dev, frac gray, frac out[])
{
    int * map = ((gx_devn_prn_device *) dev)->devn_params.separation_order_map;

    gray_cs_to_devn_cm(dev, map, gray, out);
}