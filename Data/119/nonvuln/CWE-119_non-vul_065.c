int qemu_fdt_nop_node(void *fdt, const char *node_path)
{
    int r;

    r = fdt_nop_node(fdt, findnode_nofail(fdt, node_path));
    if (r < 0) {
        error_report("%s: Couldn't nop node %s: %s", __func__, node_path,
                     fdt_strerror(r));
        exit(1);
    }

    return r;
}