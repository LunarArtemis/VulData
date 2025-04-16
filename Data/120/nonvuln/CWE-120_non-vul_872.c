int
devn_copy_params(gx_device * psrcdev, gx_device * pdesdev)
{
    gs_devn_params *src_devn_params, *des_devn_params;
    int code = 0;
    int k;

    /* Get pointers to the parameters */
    src_devn_params = dev_proc(psrcdev, ret_devn_params)(psrcdev);
    des_devn_params = dev_proc(pdesdev, ret_devn_params)(pdesdev);
    if (src_devn_params == NULL || des_devn_params == NULL)
        return gs_note_error(gs_error_undefined);

    /* First the easy items */
    des_devn_params->bitspercomponent = src_devn_params->bitspercomponent;
    des_devn_params->max_separations = src_devn_params->max_separations;
    des_devn_params->num_separation_order_names =
        src_devn_params->num_separation_order_names;
    des_devn_params->num_std_colorant_names =
        src_devn_params->num_std_colorant_names;
    des_devn_params->page_spot_colors = src_devn_params->page_spot_colors;
    des_devn_params->std_colorant_names = src_devn_params->std_colorant_names;
    des_devn_params->separations.num_separations
        = src_devn_params->separations.num_separations;
    /* Now the more complex structures */
    /* Spot color names */
    for (k = 0; k < des_devn_params->separations.num_separations; k++) {
        byte * sep_name;
        int name_size = src_devn_params->separations.names[k].size;
        sep_name = (byte *)gs_alloc_bytes(pdesdev->memory->stable_memory,
                                          name_size, "devn_copy_params");
        if (sep_name == NULL) {
            return_error(gs_error_VMerror);
        }
        memcpy(sep_name, src_devn_params->separations.names[k].data, name_size);
        des_devn_params->separations.names[k].size = name_size;
        des_devn_params->separations.names[k].data = sep_name;
    }
    /* Order map */
    memcpy(des_devn_params->separation_order_map,
           src_devn_params->separation_order_map, sizeof(gs_separation_map));

    /* Handle the PDF14 items if they are there */
    des_devn_params->pdf14_separations.num_separations
        = src_devn_params->pdf14_separations.num_separations;
    for (k = 0; k < des_devn_params->pdf14_separations.num_separations; k++) {
        byte * sep_name;
        int name_size = src_devn_params->pdf14_separations.names[k].size;
        sep_name = (byte *)gs_alloc_bytes(pdesdev->memory->stable_memory,
                                          name_size, "devn_copy_params");
        if (sep_name == NULL) {
            return_error(gs_error_VMerror);
        }
        memcpy(sep_name, src_devn_params->pdf14_separations.names[k].data,
               name_size);
        des_devn_params->pdf14_separations.names[k].size = name_size;
        des_devn_params->pdf14_separations.names[k].data = sep_name;
    }
    return code;
}