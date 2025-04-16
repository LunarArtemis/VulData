int qemu_fdt_setprop_phandle(void *fdt, const char *node_path,
                             const char *property,
                             const char *target_node_path)
{
    uint32_t phandle = qemu_fdt_get_phandle(fdt, target_node_path);
    return qemu_fdt_setprop_cell(fdt, node_path, property, phandle);
}