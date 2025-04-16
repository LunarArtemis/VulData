int
spotcmyk_prn_open(gx_device * pdev)
{
    int code = gdev_prn_open(pdev);

    while (pdev->child)
        pdev = pdev->child;

    set_linear_color_bits_mask_shift(pdev);
    pdev->color_info.separable_and_linear = GX_CINFO_SEP_LIN;
    return code;
}