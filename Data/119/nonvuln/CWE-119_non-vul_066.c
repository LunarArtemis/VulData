void qemu_fdt_dumpdtb(void *fdt, int size)
{
    const char *dumpdtb = qemu_opt_get(qemu_get_machine_opts(), "dumpdtb");

    if (dumpdtb) {
        /* Dump the dtb to a file and quit */
        exit(g_file_set_contents(dumpdtb, fdt, size, NULL) ? 0 : 1);
    }
}