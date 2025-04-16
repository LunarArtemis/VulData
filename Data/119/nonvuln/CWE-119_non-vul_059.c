int qemu_fdt_setprop_string(void *fdt, const char *node_path,
                            const char *property, const char *string)
{
    int r;

    r = fdt_setprop_string(fdt, findnode_nofail(fdt, node_path), property, string);
    if (r < 0) {
        error_report("%s: Couldn't set %s/%s = %s: %s", __func__,
                     node_path, property, string, fdt_strerror(r));
        exit(1);
    }

    return r;
}