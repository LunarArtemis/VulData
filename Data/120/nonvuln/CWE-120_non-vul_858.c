int
devn_generic_put_params(gx_device *pdev, gs_param_list *plist,
                        gs_devn_params *pdevn_params, equivalent_cmyk_color_params *pequiv_colors,
                        int is_printer)
{
    int code;
    /* Save current data in case we have a problem */
    gx_device_color_info save_info = pdev->color_info;
    gs_devn_params saved_devn_params = *pdevn_params;
    equivalent_cmyk_color_params saved_equiv_colors;
    int save_planes = pdev->num_planar_planes;

    if (pequiv_colors != NULL)
        saved_equiv_colors = *pequiv_colors;

    /* Use utility routine to handle parameters */
    code = devn_put_params(pdev, plist, pdevn_params, pequiv_colors);

    /* Check for default printer parameters */
    if (is_printer && code >= 0)
        code = gdev_prn_put_params(pdev, plist);

    /* If we have an error then restore original data. */
    if (code < 0) {
        pdev->color_info = save_info;
        *pdevn_params = saved_devn_params;
        if (pequiv_colors != NULL)
           *pequiv_colors = saved_equiv_colors;
        return code;
    }

    /* If anything changed, then close the device, etc. */
    if (!gx_color_info_equal(&pdev->color_info, &save_info) ||
        !devn_params_equal(pdevn_params, &saved_devn_params) ||
        (pequiv_colors != NULL &&
            compare_equivalent_cmyk_color_params(pequiv_colors, &saved_equiv_colors)) ||
        pdev->num_planar_planes != save_planes) {
        gx_device *parent_dev = pdev;
        gx_device_color_info resave_info = pdev->color_info;
        int resave_planes = pdev->num_planar_planes;

        while (parent_dev->parent != NULL)
            parent_dev = parent_dev->parent;

        /* Temporarily restore the old color_info, so the close happens with
         * the old version. In particular this allows Nup to flush properly. */
        pdev->color_info = save_info;
        pdev->num_planar_planes = save_planes;
        gs_closedevice(parent_dev);
        /* Then put the shiny new color_info back in. */
        pdev->color_info = resave_info;
        pdev->num_planar_planes = resave_planes;
        /* Reset the separable and linear shift, masks, bits. */
        set_linear_color_bits_mask_shift(pdev);
    }
    /*
     * Also check for parameters which are being passed from the PDF 1.4
     * compositior clist write device.  This device needs to pass info
     * to the PDF 1.4 compositor clist reader device.  However this device
     * is not crated until the clist is being read.  Thus we have to buffer
     * this info in the output device.   (This is only needed for devices
     * which support spot colors.)
     */
    code = pdf14_put_devn_params(pdev, pdevn_params, plist);
    return code;
}