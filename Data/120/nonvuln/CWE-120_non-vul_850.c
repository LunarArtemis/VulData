const gx_cm_color_map_procs *
gx_devn_prn_get_color_mapping_procs(const gx_device * dev, const gx_device **map_dev)
{
    *map_dev = dev;
    return &spotCMYK_procs;
}