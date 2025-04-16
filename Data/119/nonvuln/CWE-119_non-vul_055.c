void *load_device_tree_from_sysfs(void)
{
    void *host_fdt;
    int host_fdt_size;

    host_fdt = create_device_tree(&host_fdt_size);
    read_fstree(host_fdt, SYSFS_DT_BASEDIR);
    if (fdt_check_header(host_fdt)) {
        error_report("%s host device tree extracted into memory is invalid",
                     __func__);
        exit(1);
    }
    return host_fdt;
}