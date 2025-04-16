bool PackMachBase<T>::canPack()
{
    unsigned const lc_seg = lc_segment[sizeof(Addr)>>3];
    fi->seek(0, SEEK_SET);
    fi->readx(&mhdri, sizeof(mhdri));

    if (((unsigned) Mach_header::MH_MAGIC + (sizeof(Addr)>>3)) !=mhdri.magic
    ||  my_cputype   !=mhdri.cputype
    ||  my_filetype  !=mhdri.filetype
    )
        return false;
    my_cpusubtype = mhdri.cpusubtype;

    unsigned const sz_mhcmds = (unsigned)mhdri.sizeofcmds;
    if (16384 < sz_mhcmds) { // somewhat arbitrary, but amd64-darwin.macho-upxmain.c
        throwCantPack("16384 < Mach_header.sizeofcmds");
    }
    rawmseg = (Mach_segment_command *) New(char, sz_mhcmds);
    fi->readx(rawmseg, mhdri.sizeofcmds);

    unsigned const ncmds = mhdri.ncmds;
    if (256 < ncmds) { // arbitrary, but guard against garbage
        throwCantPack("256 < Mach_header.ncmds");
    }
    msegcmd = New(Mach_segment_command, ncmds);
    unsigned char const *ptr = (unsigned char const *)rawmseg;
    for (unsigned j= 0; j < ncmds; ++j) {
        Mach_segment_command const *segptr = (Mach_segment_command const *)ptr;
        if (lc_seg == segptr->cmd) {
            msegcmd[j] = *segptr;
            if (!strcmp("__DATA", segptr->segname)) {
                for (Mach_section_command const *secptr = (Mach_section_command const *)(1+ segptr);
                    ptr_udiff(secptr, segptr) < segptr->cmdsize;
                    ++secptr
                ) {
                    if (sizeof(Addr) == secptr->size
                    && !strcmp("__mod_init_func", secptr->sectname)) {
                        o__mod_init_func = secptr->offset;
                        fi->seek(o__mod_init_func, SEEK_SET);
                        fi->readx(&prev_mod_init_func, sizeof(Addr));
                    }
                }
            }
        }
        else {
            memcpy(&msegcmd[j], ptr, 2*sizeof(unsigned)); // cmd and size
        }
        switch (((Mach_uuid_command const *)ptr)->cmd) {
        default: break;
        case Mach_command::LC_UUID: {
            memcpy(&cmdUUID, ptr, sizeof(cmdUUID));  // remember the UUID
            // Set output UUID to be 1 more than the input UUID.
            for (unsigned k = 0; k < sizeof(cmdUUID.uuid); ++k) {
                if (0 != ++cmdUUID.uuid[k]) { // no Carry
                    break;
                }
            }
        } break;
        case Mach_command::LC_VERSION_MIN_MACOSX: {
            memcpy(&cmdVERMIN, ptr, sizeof(cmdVERMIN));
        } break;
        case Mach_command::LC_SOURCE_VERSION: {
            memcpy(&cmdSRCVER, ptr, sizeof(cmdSRCVER));
        } break;
        }
        ptr += (unsigned) ((const Mach_segment_command *)ptr)->cmdsize;
    }
    if (Mach_header::MH_DYLIB==my_filetype && 0==o__mod_init_func) {
        infoWarning("missing -init function");
        return false;
    }

    // Put LC_SEGMENT together at the beginning, ascending by .vmaddr.
    qsort(msegcmd, ncmds, sizeof(*msegcmd), compare_segment_command);

    if (lc_seg==msegcmd[0].cmd && 0==msegcmd[0].vmaddr
    &&  !strcmp("__PAGEZERO", msegcmd[0].segname)) {
        pagezero_vmsize = msegcmd[0].vmsize;
    }

    // Check alignment of non-null LC_SEGMENT.
    for (unsigned j= 0; j < ncmds; ++j) {
        if (lc_seg==msegcmd[j].cmd) {
            if (msegcmd[j].vmsize==0)
                break;  // was sorted last
            if (~PAGE_MASK & (msegcmd[j].fileoff | msegcmd[j].vmaddr)) {
                return false;
            }

            // We used to check that LC_SEGMENTS were contiguous,
            // but apparently that is not needed anymore,
            // and Google compilers generate strange layouts.

            ++n_segment;
            sz_segment = msegcmd[j].filesize + msegcmd[j].fileoff - msegcmd[0].fileoff;
        }
    }

    // info: currently the header is 36 (32+4) bytes before EOF
    unsigned char buf[256];
    fi->seek(-(off_t)sizeof(buf), SEEK_END);
    fi->readx(buf, sizeof(buf));
    checkAlreadyPacked(buf, sizeof(buf));

    // set options
    opt->o_unix.blocksize = file_size;
    if (!n_segment) {
        return false;
    }
    struct {
        unsigned cputype;
        unsigned short filetype;
        unsigned short sz_stub_entry;
        unsigned short sz_stub_fold;
        unsigned short sz_stub_main;
        upx_byte const *stub_entry;
        upx_byte const *stub_fold;
        upx_byte const *stub_main;
    } const stub_list[] = {
        {CPU_TYPE_I386, MH_EXECUTE,
            sizeof(stub_i386_darwin_macho_entry),
            sizeof(stub_i386_darwin_macho_fold),
            sizeof(stub_i386_darwin_macho_upxmain_exe),
                   stub_i386_darwin_macho_entry,
                   stub_i386_darwin_macho_fold,
                   stub_i386_darwin_macho_upxmain_exe
        },
        {CPU_TYPE_I386, MH_DYLIB,
            sizeof(stub_i386_darwin_dylib_entry), 0, 0,
                   stub_i386_darwin_dylib_entry,  0, 0
        },
        {CPU_TYPE_X86_64, MH_EXECUTE,
            sizeof(stub_amd64_darwin_macho_entry),
            sizeof(stub_amd64_darwin_macho_fold),
            sizeof(stub_amd64_darwin_macho_upxmain_exe),
                   stub_amd64_darwin_macho_entry,
                   stub_amd64_darwin_macho_fold,
                   stub_amd64_darwin_macho_upxmain_exe
        },
        {CPU_TYPE_X86_64, MH_DYLIB,
            sizeof(stub_amd64_darwin_dylib_entry), 0, 0,
                   stub_amd64_darwin_dylib_entry,  0, 0
        },
        {CPU_TYPE_ARM, MH_EXECUTE,
            sizeof(stub_arm_v5a_darwin_macho_entry),
            sizeof(stub_arm_v5a_darwin_macho_fold),
            0,
                   stub_arm_v5a_darwin_macho_entry,
                   stub_arm_v5a_darwin_macho_fold,
                   0
        },
        {CPU_TYPE_POWERPC, MH_EXECUTE,
            sizeof(stub_powerpc_darwin_macho_entry),
            sizeof(stub_powerpc_darwin_macho_fold),
            sizeof(stub_powerpc_darwin_macho_upxmain_exe),
                   stub_powerpc_darwin_macho_entry,
                   stub_powerpc_darwin_macho_fold,
                   stub_powerpc_darwin_macho_upxmain_exe
        },
        {CPU_TYPE_POWERPC, MH_DYLIB,
            sizeof(stub_powerpc_darwin_dylib_entry), 0, 0,
                   stub_powerpc_darwin_dylib_entry,  0, 0
        },
        {CPU_TYPE_POWERPC64LE, MH_EXECUTE,
            sizeof(stub_powerpc64le_darwin_macho_entry),
            sizeof(stub_powerpc64le_darwin_macho_fold),
            0,
                   stub_powerpc64le_darwin_macho_entry,
                   stub_powerpc64le_darwin_macho_fold,
                   0
        },
        {CPU_TYPE_POWERPC64LE, MH_DYLIB,
            sizeof(stub_powerpc64le_darwin_dylib_entry), 0, 0,
                   stub_powerpc64le_darwin_dylib_entry,  0, 0
        },
        {0,0, 0,0,0, 0,0,0}
    };
    for (unsigned j = 0; stub_list[j].cputype; ++j) {
        if (stub_list[j].cputype  == my_cputype
        &&  stub_list[j].filetype == my_filetype) {
            sz_stub_entry = stub_list[j].sz_stub_entry;
               stub_entry = stub_list[j].stub_entry;
            sz_stub_fold  = stub_list[j].sz_stub_fold;
               stub_fold  = stub_list[j].stub_fold;
            sz_stub_main  = stub_list[j].sz_stub_main;
               stub_main  = stub_list[j].stub_main;
        }
    }
    return true;
}