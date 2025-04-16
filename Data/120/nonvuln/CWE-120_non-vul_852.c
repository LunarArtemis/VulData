int
check_pcm_and_separation_names(const gx_device * dev,
                const gs_devn_params * pparams, const char * pname,
                int name_size, int component_type)
{
    fixed_colorant_name * pcolor = pparams->std_colorant_names;
    int color_component_number = 0;

    /* Check if the component is in the process color model list. */
    if (pcolor) {
        while( *pcolor) {
            if (compare_color_names(pname, name_size, *pcolor, strlen(*pcolor)))
                return color_component_number;
            pcolor++;
            color_component_number++;
        }
    }
    /* For some devices, Tags is part of the process color model list. If so,
     * that throws us off here since it is thrown at the end of the list. Adjust. */
    if (device_encodes_tags(dev)) {
        color_component_number--;
    }

    return check_separation_names(dev, pparams, pname, name_size,
        component_type, color_component_number);
}