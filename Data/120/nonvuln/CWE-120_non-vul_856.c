int
gx_devn_prn_update_spot_equivalent_colors(gx_device *dev, const gs_gstate * pgs, const gs_color_space *pcs)
{
    gx_devn_prn_device *pdev = (gx_devn_prn_device *)dev;

    return update_spot_equivalent_cmyk_colors(dev, pgs, pcs, &pdev->devn_params,
                                              &pdev->equiv_cmyk_colors);
}