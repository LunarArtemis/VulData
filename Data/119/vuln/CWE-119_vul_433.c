static void elf_load_bsdsyms(struct elf_binary *elf)
{
    /*
     * Header that is placed at the end of the kernel and allows
     * the OS to find where the symtab and strtab have been loaded.
     * It mimics a valid ELF file header, although it only contains
     * a symtab and a strtab section. The layout in memory is the
     * following:
     *
     * +------------------------+
     * |                        |
     * | KERNEL                 |
     * |                        |
     * +------------------------+ pend
     * | ALIGNMENT              |
     * +------------------------+ bsd_symtab_pstart
     * |                        |
     * | size                   | bsd_symtab_pend - bsd_symtab_pstart
     * |                        |
     * +------------------------+ bsd_symtab_pstart + 4
     * |                        |
     * | ELF header             |
     * |                        |
     * +------------------------+
     * |                        |
     * | ELF section header 0   | Undefined section header
     * |                        |
     * +------------------------+
     * |                        |
     * | ELF section header 1   | SYMTAB section header
     * |                        |
     * +------------------------+
     * |                        |
     * | ELF section header 2   | STRTAB section header
     * |                        |
     * +------------------------+
     * |                        |
     * | SYMTAB                 |
     * |                        |
     * +------------------------+
     * |                        |
     * | STRTAB                 |
     * |                        |
     * +------------------------+ bsd_symtab_pend
     *
     * NB: according to the ELF spec there's only ONE symtab per ELF
     * file, and accordingly we will only load the corresponding
     * strtab, so we only need three section headers in our fake ELF
     * header (first section header is always the undefined section).
     */
    struct {
        uint32_t size;
        struct {
            elf_ehdr header;
            elf_shdr section[ELF_BSDSYM_SECTIONS];
        } __attribute__((packed)) elf_header;
    } __attribute__((packed)) header;

    ELF_HANDLE_DECL(elf_ehdr) header_handle;
    unsigned long shdr_size;
    ELF_HANDLE_DECL(elf_shdr) section_handle;
    unsigned int link, rc;
    elf_ptrval header_base;
    elf_ptrval elf_header_base;
    elf_ptrval symtab_base;
    elf_ptrval strtab_base;

    if ( !elf->bsd_symtab_pstart )
        return;

#define elf_store_field_bitness(_elf, _hdr, _elm, _val)             \
do {                                                                \
    if ( elf_64bit(_elf) )                                          \
        elf_store_field(_elf, _hdr, e64._elm, _val);                \
    else                                                            \
        elf_store_field(_elf, _hdr, e32._elm, _val);                \
} while ( 0 )

#define SYMTAB_INDEX    1
#define STRTAB_INDEX    2

    /* Allow elf_memcpy_safe to write to symbol_header. */
    elf->caller_xdest_base = &header;
    elf->caller_xdest_size = sizeof(header);

    /*
     * Calculate the position of the various elements in GUEST MEMORY SPACE.
     * This addresses MUST only be used with elf_load_image.
     *
     * NB: strtab_base cannot be calculated at this point because we don't
     * know the size of the symtab yet, and the strtab will be placed after it.
     */
    header_base = elf_get_ptr(elf, elf->bsd_symtab_pstart);
    elf_header_base = elf_get_ptr(elf, elf->bsd_symtab_pstart) +
                      sizeof(uint32_t);
    symtab_base = elf_round_up(elf, header_base + sizeof(header));

    /* Fill the ELF header, copied from the original ELF header. */
    header_handle = ELF_MAKE_HANDLE(elf_ehdr,
                                ELF_REALPTR2PTRVAL(&header.elf_header.header));
    elf_memcpy_safe(elf, ELF_HANDLE_PTRVAL(header_handle),
                    ELF_HANDLE_PTRVAL(elf->ehdr),
                    elf_uval(elf, elf->ehdr, e_ehsize));

    /* Set the offset to the shdr array. */
    elf_store_field_bitness(elf, header_handle, e_shoff,
                            offsetof(typeof(header.elf_header), section));

    /* Set the right number of section headers. */
    elf_store_field_bitness(elf, header_handle, e_shnum, ELF_BSDSYM_SECTIONS);

    /* Clear a couple of fields we don't use. */
    elf_store_field_bitness(elf, header_handle, e_phoff, 0);
    elf_store_field_bitness(elf, header_handle, e_phentsize, 0);
    elf_store_field_bitness(elf, header_handle, e_phnum, 0);

    /* Zero the undefined section. */
    section_handle = ELF_MAKE_HANDLE(elf_shdr,
                     ELF_REALPTR2PTRVAL(&header.elf_header.section[SHN_UNDEF]));
    shdr_size = elf_uval(elf, elf->ehdr, e_shentsize);
    elf_memset_safe(elf, ELF_HANDLE_PTRVAL(section_handle), 0, shdr_size);

    /*
     * The symtab section header is going to reside in section[SYMTAB_INDEX],
     * while the corresponding strtab is going to be placed in
     * section[STRTAB_INDEX]. sh_offset is mangled so it points to the offset
     * where the sections are actually loaded (relative to the ELF header
     * location).
     */
    section_handle = ELF_MAKE_HANDLE(elf_shdr,
                ELF_REALPTR2PTRVAL(&header.elf_header.section[SYMTAB_INDEX]));

    elf_memcpy_safe(elf, ELF_HANDLE_PTRVAL(section_handle),
                    ELF_HANDLE_PTRVAL(elf->sym_tab),
                    shdr_size);

    /* Copy the original sh_link field before mangling it. */
    link = elf_uval(elf, section_handle, sh_link);

    /* Load symtab into guest memory. */
    rc = elf_load_image(elf, symtab_base,
                        elf_section_start(elf, section_handle),
                        elf_uval(elf, section_handle, sh_size),
                        elf_uval(elf, section_handle, sh_size));
    if ( rc != 0 )
    {
        elf_mark_broken(elf, "unable to load symtab into guest memory");
        return;
    }

    /* Adjust the sh_offset and sh_link of the copied section header. */
    elf_store_field_bitness(elf, section_handle, sh_offset,
                            symtab_base - elf_header_base);
    elf_store_field_bitness(elf, section_handle, sh_link,
                            STRTAB_INDEX);

    /* Calculate the guest address where strtab is loaded. */
    strtab_base = elf_round_up(elf, symtab_base +
                               elf_uval(elf, section_handle, sh_size));

    /* Load strtab section header. */
    section_handle = ELF_MAKE_HANDLE(elf_shdr,
            ELF_REALPTR2PTRVAL(&header.elf_header.section[STRTAB_INDEX]));
    elf_memcpy_safe(elf, ELF_HANDLE_PTRVAL(section_handle),
                    ELF_HANDLE_PTRVAL(elf_shdr_by_index(elf, link)),
                    shdr_size);

    if ( elf_uval(elf, section_handle, sh_type) != SHT_STRTAB )
    {
        elf_mark_broken(elf, "strtab not found");
        return;
    }

    /* Load strtab into guest memory. */
    rc = elf_load_image(elf, strtab_base,
                        elf_section_start(elf, section_handle),
                        elf_uval(elf, section_handle, sh_size),
                        elf_uval(elf, section_handle, sh_size));
    if ( rc != 0 )
    {
        elf_mark_broken(elf, "unable to load strtab into guest memory");
        return;
    }

    elf_store_field_bitness(elf, section_handle, sh_offset,
                            strtab_base - elf_header_base);

    /* Store the whole size (including headers and loaded sections). */
    header.size = strtab_base + elf_uval(elf, section_handle, sh_size) -
                  elf_header_base;

    /* Load the headers. */
    rc = elf_load_image(elf, header_base, ELF_REALPTR2PTRVAL(&header),
                        sizeof(header), sizeof(header));
    if ( rc != 0 )
    {
        elf_mark_broken(elf, "unable to load ELF headers into guest memory");
        return;
    }

    /* Remove permissions from elf_memcpy_safe. */
    elf->caller_xdest_base = NULL;
    elf->caller_xdest_size = 0;

#undef SYMTAB_INDEX
#undef STRTAB_INDEX
#undef elf_store_field_bitness
}