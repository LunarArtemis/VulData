elf_errorstatus elf_init(struct elf_binary *elf, const char *image_input, size_t size)
{
    ELF_HANDLE_DECL(elf_shdr) shdr;
    uint64_t i, count, section, offset, link;

    if ( !elf_is_elfbinary(image_input, size) )
    {
        elf_err(elf, "ELF: not an ELF binary\n");
        return -1;
    }

    elf_memset_unchecked(elf, 0, sizeof(*elf));
    elf->image_base = image_input;
    elf->size = size;
    elf->ehdr = ELF_MAKE_HANDLE(elf_ehdr, (elf_ptrval)image_input);
    elf->class = elf_uval_3264(elf, elf->ehdr, e32.e_ident[EI_CLASS]);
    elf->data = elf_uval_3264(elf, elf->ehdr, e32.e_ident[EI_DATA]);
    elf->caller_xdest_base = NULL;
    elf->caller_xdest_size = 0;

    /* Sanity check phdr. */
    offset = elf_uval(elf, elf->ehdr, e_phoff) +
        elf_uval(elf, elf->ehdr, e_phentsize) * elf_phdr_count(elf);
    if ( offset > elf->size )
    {
        elf_err(elf, "ELF: phdr overflow (off %" PRIx64 " > size %lx)\n",
                offset, (unsigned long)elf->size);
        return -1;
    }

    /* Sanity check shdr. */
    offset = elf_uval(elf, elf->ehdr, e_shoff) +
        elf_uval(elf, elf->ehdr, e_shentsize) * elf_shdr_count(elf);
    if ( offset > elf->size )
    {
        elf_err(elf, "ELF: shdr overflow (off %" PRIx64 " > size %lx)\n",
                offset, (unsigned long)elf->size);
        return -1;
    }

    /* Find section string table. */
    section = elf_uval(elf, elf->ehdr, e_shstrndx);
    shdr = elf_shdr_by_index(elf, section);
    if ( ELF_HANDLE_VALID(shdr) )
        elf->sec_strtab = elf_section_start(elf, shdr);

    /* Find symbol table and symbol string table. */
    count = elf_shdr_count(elf);
    for ( i = 0; i < count; i++ )
    {
        shdr = elf_shdr_by_index(elf, i);
        if ( !elf_access_ok(elf, ELF_HANDLE_PTRVAL(shdr), 1) )
            /* input has an insane section header count field */
            break;
        if ( elf_uval(elf, shdr, sh_type) != SHT_SYMTAB )
            continue;

        link = elf_uval(elf, shdr, sh_link);
        if ( link == SHN_UNDEF || link >= elf_shdr_count(elf) )
            /* out-of-bounds link value. */
            break;

        elf->sym_tab = shdr;
        shdr = elf_shdr_by_index(elf, link);
        if ( !ELF_HANDLE_VALID(shdr) )
        {
            elf->sym_tab = ELF_INVALID_HANDLE(elf_shdr);
            continue;
        }
        elf->sym_strtab = elf_section_start(elf, shdr);
        break;
    }

    return 0;
}