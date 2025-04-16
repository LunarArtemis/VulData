uint32_t qemu_fdt_alloc_phandle(void *fdt)
{
    static int phandle = 0x0;

    /*
     * We need to find out if the user gave us special instruction at
     * which phandle id to start allocating phandles.
     */
    if (!phandle) {
        phandle = machine_phandle_start(current_machine);
    }

    if (!phandle) {
        /*
         * None or invalid phandle given on the command line, so fall back to
         * default starting point.
         */
        phandle = 0x8000;
    }

    return phandle++;
}