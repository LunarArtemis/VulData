void build_cmyk_map(gx_device *pdev, int num_comp,
    equivalent_cmyk_color_params *equiv_cmyk_colors,
    cmyk_composite_map * cmyk_map)
{
    int comp_num;
    gs_devn_params *devn_params =  dev_proc(pdev, ret_devn_params)(pdev);

    if (devn_params == NULL)
        return;

    for (comp_num = 0; comp_num < num_comp; comp_num++) {
        int sep_num = devn_params->separation_order_map[comp_num];

        cmyk_map[comp_num].c = cmyk_map[comp_num].m =
            cmyk_map[comp_num].y = cmyk_map[comp_num].k = frac_0;
        /* The tiffsep device has 4 standard colors:  CMYK */
        if (sep_num < devn_params->num_std_colorant_names) {
            switch (sep_num) {
            case 0: cmyk_map[comp_num].c = frac_1; break;
            case 1: cmyk_map[comp_num].m = frac_1; break;
            case 2: cmyk_map[comp_num].y = frac_1; break;
            case 3: cmyk_map[comp_num].k = frac_1; break;
            }
        } else {
            sep_num -= devn_params->num_std_colorant_names;
            if (equiv_cmyk_colors->color[sep_num].color_info_valid) {
                cmyk_map[comp_num].c = equiv_cmyk_colors->color[sep_num].c;
                cmyk_map[comp_num].m = equiv_cmyk_colors->color[sep_num].m;
                cmyk_map[comp_num].y = equiv_cmyk_colors->color[sep_num].y;
                cmyk_map[comp_num].k = equiv_cmyk_colors->color[sep_num].k;
            }
        }
    }
}