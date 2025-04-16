gs_devn_params *
gx_devn_prn_ret_devn_params(gx_device * dev)
{
    gx_devn_prn_device *pdev = (gx_devn_prn_device *)dev;

    return &pdev->devn_params;
}