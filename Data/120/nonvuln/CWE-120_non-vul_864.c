int
gx_devn_prn_get_color_comp_index(gx_device * dev, const char * pname,
                                        int name_size, int component_type)
{
    gx_devn_prn_device *pdev = (gx_devn_prn_device *)dev;

    return devn_get_color_comp_index(dev,
                                     &pdev->devn_params,
                                     &pdev->equiv_cmyk_colors,
                                     pname,
                                     name_size,
                                     component_type,
                                     ENABLE_AUTO_SPOT_COLORS);
}