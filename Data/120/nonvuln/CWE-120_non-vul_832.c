int
devn_printer_put_params(gx_device *pdev, gs_param_list *plist,
        gs_devn_params *pdevn_params, equivalent_cmyk_color_params *pequiv_colors)
{
    return devn_generic_put_params(pdev, plist, pdevn_params, pequiv_colors, 1);
}