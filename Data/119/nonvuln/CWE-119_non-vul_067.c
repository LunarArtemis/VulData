int qemu_fdt_setprop_cell(void *fdt, const char *node_path,
                          const char *property, uint32_t val)
{
    int r;

    r = fdt_setprop_cell(fdt, findnode_nofail(fdt, node_path), property, val);
    if (r < 0) {
        error_report("%s: Couldn't set %s/%s = %#08x: %s", __func__,
                     node_path, property, val, fdt_strerror(r));
        exit(1);
    }

    return r;
}