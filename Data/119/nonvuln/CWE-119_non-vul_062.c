int qemu_fdt_setprop(void *fdt, const char *node_path,
                     const char *property, const void *val, int size)
{
    int r;

    r = fdt_setprop(fdt, findnode_nofail(fdt, node_path), property, val, size);
    if (r < 0) {
        error_report("%s: Couldn't set %s/%s: %s", __func__, node_path,
                     property, fdt_strerror(r));
        exit(1);
    }

    return r;
}