int
gx_devn_prn_get_params(gx_device *dev, gs_param_list *plist)
{
    gx_devn_prn_device *pdev = (gx_devn_prn_device *)dev;
    int code = gdev_prn_get_params(dev, plist);

    if (code < 0)
        return code;
    return devn_get_params(dev, plist, &pdev->devn_params,
                           &pdev->equiv_cmyk_colors);
}