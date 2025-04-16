char **qemu_fdt_node_path(void *fdt, const char *name, char *compat,
                          Error **errp)
{
    int offset, len, ret;
    const char *iter_name;
    unsigned int path_len = 16, n = 0;
    GSList *path_list = NULL, *iter;
    char **path_array;

    offset = fdt_node_offset_by_compatible(fdt, -1, compat);

    while (offset >= 0) {
        iter_name = fdt_get_name(fdt, offset, &len);
        if (!iter_name) {
            offset = len;
            break;
        }
        if (!strcmp(iter_name, name)) {
            char *path;

            path = g_malloc(path_len);
            while ((ret = fdt_get_path(fdt, offset, path, path_len))
                  == -FDT_ERR_NOSPACE) {
                path_len += 16;
                path = g_realloc(path, path_len);
            }
            path_list = g_slist_prepend(path_list, path);
            n++;
        }
        offset = fdt_node_offset_by_compatible(fdt, offset, compat);
    }

    if (offset < 0 && offset != -FDT_ERR_NOTFOUND) {
        error_setg(errp, "%s: abort parsing dt for %s/%s: %s",
                   __func__, name, compat, fdt_strerror(offset));
        for (iter = path_list; iter; iter = iter->next) {
            g_free(iter->data);
        }
        g_slist_free(path_list);
        return NULL;
    }

    path_array = g_new(char *, n + 1);
    path_array[n--] = NULL;

    for (iter = path_list; iter; iter = iter->next) {
        path_array[n--] = iter->data;
    }

    g_slist_free(path_list);

    return path_array;
}