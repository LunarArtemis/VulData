void *create_device_tree(int *sizep)
{
    void *fdt;
    int ret;

    *sizep = FDT_MAX_SIZE;
    fdt = g_malloc0(FDT_MAX_SIZE);
    ret = fdt_create(fdt, FDT_MAX_SIZE);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_finish_reservemap(fdt);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_begin_node(fdt, "");
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_end_node(fdt);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_finish(fdt);
    if (ret < 0) {
        goto fail;
    }
    ret = fdt_open_into(fdt, fdt, *sizep);
    if (ret) {
        error_report("Unable to copy device tree in memory");
        exit(1);
    }

    return fdt;
fail:
    error_report("%s Couldn't create dt: %s", __func__, fdt_strerror(ret));
    exit(1);
}