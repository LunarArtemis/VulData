int
devn_get_color_comp_index(gx_device * dev, gs_devn_params * pdevn_params,
                    equivalent_cmyk_color_params * pequiv_colors,
                    const char * pname, int name_size, int component_type,
                    int auto_spot_colors)
{
    int num_order = pdevn_params->num_separation_order_names;
    int color_component_number = 0;
    int num_res_comps = pdevn_params->num_reserved_components;
    int max_spot_colors = GX_DEVICE_MAX_SEPARATIONS - pdevn_params->num_std_colorant_names - num_res_comps;

    /*
     * Check if the component is in either the process color model list
     * or in the SeparationNames list.
     */
    color_component_number = check_pcm_and_separation_names(dev, pdevn_params,
                                        pname, name_size, component_type);

    /* If we have a valid component */
    if (color_component_number >= 0) {
        /* Check if the component is in the separation order map. */
        if (num_order)
            color_component_number =
                pdevn_params->separation_order_map[color_component_number];
        else
            /*
             * We can have more spot colors than we can image.  We simply
             * ignore the component (i.e. treat it the same as we would
             * treat a component that is not in the separation order map).
             * Note:  Most device do not allow more spot colors than we can
             * image.  (See the options for auto_spot_color in gdevdevn.h.)
             */
            if (color_component_number >= dev->color_info.max_components)
                color_component_number = GX_DEVICE_COLOR_MAX_COMPONENTS;

        return color_component_number;
    }
    /*
     * The given name does not match any of our current components or
     * separations.  Check if we should add the spot color to our list.
     * If the SeparationOrder parameter has been specified then we should
     * already have our complete list of desired spot colorants.
     */
    if (component_type != SEPARATION_NAME ||
            auto_spot_colors == NO_AUTO_SPOT_COLORS ||
            pdevn_params->num_separation_order_names != 0)
        return -1;      /* Do not add --> indicate colorant unknown. */

    /* Make sure the name is not "None"  this is sometimes
       within a DeviceN list and should not be added as one of the
       separations.  */
    if (strncmp(pname, "None", name_size) == 0) {
        return -1;
    }

    /*
     * Check if we have room for another spot colorant.
     */
    if (auto_spot_colors == ENABLE_AUTO_SPOT_COLORS)
        /* limit max_spot_colors to what the device can handle given max_components */
        max_spot_colors = min(max_spot_colors,
                              dev->color_info.max_components - pdevn_params->num_std_colorant_names - num_res_comps);
    if (pdevn_params->separations.num_separations < max_spot_colors) {
        byte * sep_name;
        gs_separations * separations = &pdevn_params->separations;
        int sep_num = separations->num_separations++;
        /* We have a new spot colorant - put in stable memory to avoid "restore" */
        sep_name = gs_alloc_bytes(dev->memory->stable_memory, name_size, "devn_get_color_comp_index");
        if (sep_name == NULL) {
            separations->num_separations--;	/* we didn't add it */
            return -1;
        }
        memcpy(sep_name, pname, name_size);
        separations->names[sep_num].size = name_size;
        separations->names[sep_num].data = sep_name;
        color_component_number = sep_num + pdevn_params->num_std_colorant_names;
        if (color_component_number >= dev->color_info.max_components)
            color_component_number = GX_DEVICE_COLOR_MAX_COMPONENTS;
        else
            pdevn_params->separation_order_map[color_component_number] =
                                               color_component_number;

        if (pequiv_colors != NULL) {
            /* Indicate that we need to find equivalent CMYK color. */
            pequiv_colors->color[sep_num].color_info_valid = false;
            pequiv_colors->all_color_info_valid = false;
        }
    }

    return color_component_number;
}