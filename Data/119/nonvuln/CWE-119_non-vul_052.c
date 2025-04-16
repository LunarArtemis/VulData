uint32_t qemu_fdt_getprop_cell(void *fdt, const char *node_path,
                               const char *property, int *lenp, Error **errp)
{
    int len;
    const uint32_t *p;

    if (!lenp) {
        lenp = &len;
    }
    p = qemu_fdt_getprop(fdt, node_path, property, lenp, errp);
    if (!p) {
        return 0;
    } else if (*lenp != 4) {
        error_setg(errp, "%s: %s/%s not 4 bytes long (not a cell?)",
                   __func__, node_path, property);
        *lenp = -EINVAL;
        return 0;
    }
    return be32_to_cpu(*p);
}