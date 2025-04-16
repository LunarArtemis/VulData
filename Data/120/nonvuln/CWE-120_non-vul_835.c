int
devn_get_params(gx_device * pdev, gs_param_list * plist,
    gs_devn_params * pdevn_params, equivalent_cmyk_color_params * pequiv_colors)
{
    int code, i = 0, spot_num;
    bool seprs = false;
    gs_param_string_array scna;
    gs_param_string_array sona;
    gs_param_int_array equiv_cmyk;
    /* there are 5 ints  per colorant in equiv_elements: a valid flag and an int for C, M, Y and K */
    int equiv_elements[5 * GX_DEVICE_MAX_SEPARATIONS] = { 0 }; /* 5 * max_colors */
    /* limit in case num_separations in pdevn_params exceeds what is expected. */
    int num_separations = min(pdevn_params->separations.num_separations, sizeof(equiv_elements)/(5*sizeof(int)));


    set_param_array(scna, NULL, 0);
    set_param_array(sona, NULL, 0);

    if (pequiv_colors != NULL) {
        for (spot_num = 0; spot_num < num_separations; spot_num++) {
            equiv_elements[i++] = pequiv_colors->color[spot_num].color_info_valid ? 1 : 0;
            equiv_elements[i++] = pequiv_colors->color[spot_num].c;
            equiv_elements[i++] = pequiv_colors->color[spot_num].m;
            equiv_elements[i++] = pequiv_colors->color[spot_num].y;
            equiv_elements[i++] = pequiv_colors->color[spot_num].k;
        }
    }

    equiv_cmyk.data = equiv_elements;
    equiv_cmyk.size = i;
    equiv_cmyk.persistent = false;

    if ( (code = sample_device_crd_get_params(pdev, plist, "CRDDefault")) < 0 ||
         (code = param_write_name_array(plist, "SeparationColorNames", &scna)) < 0 ||
         (code = param_write_name_array(plist, "SeparationOrder", &sona)) < 0 ||
         (code = param_write_bool(plist, "Separations", &seprs)) < 0)
        return code;

    if (pdev->color_info.polarity == GX_CINFO_POLARITY_SUBTRACTIVE &&
        (code = param_write_int(plist, "PageSpotColors", &(pdevn_params->page_spot_colors))) < 0)
        return code;

    if (pdevn_params->separations.num_separations > 0)
        code = param_write_int_array(plist, ".EquivCMYKColors", &equiv_cmyk);

    return code;
}