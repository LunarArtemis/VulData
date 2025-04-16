const void *qemu_fdt_getprop(void *fdt, const char *node_path,
                             const char *property, int *lenp, Error **errp)
{
    int len;
    const void *r;

    if (!lenp) {
        lenp = &len;
    }
    r = fdt_getprop(fdt, findnode_nofail(fdt, node_path), property, lenp);
    if (!r) {
        error_setg(errp, "%s: Couldn't get %s/%s: %s", __func__,
                  node_path, property, fdt_strerror(*lenp));
    }
    return r;
}