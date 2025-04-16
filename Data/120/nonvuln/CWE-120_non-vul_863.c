const gs_devn_params *
gx_devn_prn_ret_devn_params_const(const gx_device * dev)
{
    const gx_devn_prn_device *pdev = (const gx_devn_prn_device *)dev;

    return &pdev->devn_params;
}