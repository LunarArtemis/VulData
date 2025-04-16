char **qemu_fdt_node_unit_path(void *fdt, const char *name, Error **errp)
{
    char *prefix =  g_strdup_printf("%s@", name);
    unsigned int path_len = 16, n = 0;
    GSList *path_list = NULL, *iter;
    const char *iter_name;
    int offset, len, ret;
    char **path_array;

    offset = fdt_next_node(fdt, -1, NULL);

    while (offset >= 0) {
        iter_name = fdt_get_name(fdt, offset, &len);
        if (!iter_name) {
            offset = len;
            break;
        }
        if (!strcmp(iter_name, name) || g_str_has_prefix(iter_name, prefix)) {
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
        offset = fdt_next_node(fdt, offset, NULL);
    }
    g_free(prefix);

    if (offset < 0 && offset != -FDT_ERR_NOTFOUND) {
        error_setg(errp, "%s: abort parsing dt for %s node units: %s",
                   __func__, name, fdt_strerror(offset));
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