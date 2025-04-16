int PackVmlinuxBase<T>::canUnpack()
{
    fi->seek(0, SEEK_SET);
    fi->readx(&ehdri, sizeof(ehdri));

    // now check the ELF header
    if (memcmp(&ehdri, "\x7f\x45\x4c\x46", 4)
    ||  ehdri.e_ident[Ehdr::EI_CLASS] != my_elfclass
    ||  ehdri.e_ident[Ehdr::EI_DATA] != my_elfdata
    ||  ehdri.e_ident[Ehdr::EI_VERSION] != Ehdr::EV_CURRENT
    ||  !memcmp(&ehdri.e_ident[8], "FreeBSD", 7)  // branded
    ||  ehdri.e_machine != my_e_machine
    ||  ehdri.e_version != 1  // version
    ||  ehdri.e_ehsize != sizeof(ehdri)  // different <elf.h> ?
    )
        return false;

    if (ehdri.e_type != Ehdr::ET_REL
    //i386 fails  ||  ehdri.e_shoff != sizeof(ehdri)  // Shdr not contiguous with Ehdr
    ||  ehdri.e_shentsize!=sizeof(Shdr)
    ||  ehdri.e_shnum < 4
    ||  (unsigned)file_size < (ehdri.e_shnum * sizeof(Shdr) + ehdri.e_shoff)
    )
        return false;

    // find the .shstrtab section
    Shdr const *const shstrsec = getElfSections();
    if (0==shstrsec) {
        return false;
    }

    // check for .text .note .note  and sane (.sh_size + .sh_offset)
    p_note0 = p_note1 = p_text = 0;
    int j;
    Shdr *p;
    for (p= shdri, j= ehdri.e_shnum; --j>=0; ++p) {
        if ((unsigned)file_size < (p->sh_size + p->sh_offset)
        ||  shstrsec->sh_size < (5+ p->sh_name) ) {
            continue;
        }
        if (0==strcmp(".text", shstrtab + p->sh_name)) {
            p_text = p;
        }
        if (0==strcmp(".note", shstrtab + p->sh_name)) {
            if (0==p_note0) {
                p_note0 = p;
            } else
            if (0==p_note1) {
                p_note1 = p;
            }
        }
    }
    if (0==p_text || 0==p_note0 || 0==p_note1) {
        return false;
    }

    char buf[1024];
    fi->seek(p_text->sh_offset + p_text->sh_size - sizeof(buf), SEEK_SET);
    fi->readx(buf, sizeof(buf));
    if (!getPackHeader(buf, sizeof(buf)))
        return -1; // format is known, but definitely is not packed

    return true;
}