int qemu_fdt_setprop_u64(void *fdt, const char *node_path,
                         const char *property, uint64_t val)
{
    val = cpu_to_be64(val);
    return qemu_fdt_setprop(fdt, node_path, property, &val, sizeof(val));
}