static void
devicen_initialize_device_procs(gx_device *dev)
{
    set_dev_proc(dev, open_device, spotcmyk_prn_open);
    set_dev_proc(dev, output_page, gdev_prn_output_page);
    set_dev_proc(dev, close_device, gdev_prn_close);
    set_dev_proc(dev, get_params, gx_devn_prn_get_params);
    set_dev_proc(dev, put_params, gx_devn_prn_put_params);
    set_dev_proc(dev, get_page_device, gx_page_device_get_page_device);
    set_dev_proc(dev, get_color_mapping_procs, gx_devn_prn_get_color_mapping_procs);
    set_dev_proc(dev, get_color_comp_index, gx_devn_prn_get_color_comp_index);
    set_dev_proc(dev, encode_color, gx_devn_prn_encode_color);
    set_dev_proc(dev, decode_color, gx_devn_prn_decode_color);
    set_dev_proc(dev, update_spot_equivalent_colors, gx_devn_prn_update_spot_equivalent_colors);
    set_dev_proc(dev, ret_devn_params, gx_devn_prn_ret_devn_params);
}