static int findnode_nofail(void *fdt, const char *node_path)
{
    int offset;

    offset = fdt_path_offset(fdt, node_path);
    if (offset < 0) {
        error_report("%s Couldn't find node %s: %s", __func__, node_path,
                     fdt_strerror(offset));
        exit(1);
    }

    return offset;
}