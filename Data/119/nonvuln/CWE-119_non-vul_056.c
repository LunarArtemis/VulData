static void read_fstree(void *fdt, const char *dirname)
{
    DIR *d;
    struct dirent *de;
    struct stat st;
    const char *root_dir = SYSFS_DT_BASEDIR;
    const char *parent_node;

    if (strstr(dirname, root_dir) != dirname) {
        error_report("%s: %s must be searched within %s",
                     __func__, dirname, root_dir);
        exit(1);
    }
    parent_node = &dirname[strlen(SYSFS_DT_BASEDIR)];

    d = opendir(dirname);
    if (!d) {
        error_report("%s cannot open %s", __func__, dirname);
        exit(1);
    }

    while ((de = readdir(d)) != NULL) {
        char *tmpnam;

        if (!g_strcmp0(de->d_name, ".")
            || !g_strcmp0(de->d_name, "..")) {
            continue;
        }

        tmpnam = g_strdup_printf("%s/%s", dirname, de->d_name);

        if (lstat(tmpnam, &st) < 0) {
            error_report("%s cannot lstat %s", __func__, tmpnam);
            exit(1);
        }

        if (S_ISREG(st.st_mode)) {
            gchar *val;
            gsize len;

            if (!g_file_get_contents(tmpnam, &val, &len, NULL)) {
                error_report("%s not able to extract info from %s",
                             __func__, tmpnam);
                exit(1);
            }

            if (strlen(parent_node) > 0) {
                qemu_fdt_setprop(fdt, parent_node,
                                 de->d_name, val, len);
            } else {
                qemu_fdt_setprop(fdt, "/", de->d_name, val, len);
            }
            g_free(val);
        } else if (S_ISDIR(st.st_mode)) {
            char *node_name;

            node_name = g_strdup_printf("%s/%s",
                                        parent_node, de->d_name);
            qemu_fdt_add_subnode(fdt, node_name);
            g_free(node_name);
            read_fstree(fdt, tmpnam);
        }

        g_free(tmpnam);
    }

    closedir(d);
}