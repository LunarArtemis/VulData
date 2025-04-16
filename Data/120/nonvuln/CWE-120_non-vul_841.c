int
gx_devn_prn_put_params(gx_device *dev, gs_param_list *plist)
{
    gx_devn_prn_device *pdev = (gx_devn_prn_device *)dev;

    return devn_printer_put_params(dev, plist, &pdev->devn_params,
                                   &pdev->equiv_cmyk_colors);
}