bool elf_access_ok(struct elf_binary * elf,
                  uint64_t ptrval, size_t size)
{
    if ( elf_ptrval_in_range(ptrval, size, elf->image_base, elf->size) )
        return 1;
    if ( elf_ptrval_in_range(ptrval, size, elf->dest_base, elf->dest_size) )
        return 1;
    if ( elf_ptrval_in_range(ptrval, size,
                             elf->caller_xdest_base, elf->caller_xdest_size) )
        return 1;
    elf_mark_broken(elf, "out of range access");
    return 0;
}