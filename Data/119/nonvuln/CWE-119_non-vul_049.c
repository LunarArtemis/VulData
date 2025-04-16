uint32_t qemu_fdt_get_phandle(void *fdt, const char *path)
{
    uint32_t r;

    r = fdt_get_phandle(fdt, findnode_nofail(fdt, path));
    if (r == 0) {
        error_report("%s: Couldn't get phandle for %s: %s", __func__,
                     path, fdt_strerror(r));
        exit(1);
    }

    return r;
}