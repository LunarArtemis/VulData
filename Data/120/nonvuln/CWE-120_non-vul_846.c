int
check_separation_names(const gx_device * dev, const gs_devn_params * pparams,
    const char * pname, int name_size, int component_type, int number)
{
    const gs_separations * separations = &pparams->separations;
    int num_spot = separations->num_separations;
    int color_component_number = number;
    int i;

    for (i = 0; i<num_spot; i++) {
        if (compare_color_names((const char *)separations->names[i].data,
            separations->names[i].size, pname, name_size)) {
            return color_component_number;
        }
        color_component_number++;
    }
    return -1;
}