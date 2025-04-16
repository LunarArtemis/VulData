void PackMachBase<T>::unpack(OutputFile *fo)
{
    unsigned const lc_seg = lc_segment[sizeof(Addr)>>3];
    fi->seek(0, SEEK_SET);
    fi->readx(&mhdri, sizeof(mhdri));
    if ((MH_MAGIC + (sizeof(Addr)>>3)) != mhdri.magic
    &&  Mach_fat_header::FAT_MAGIC != mhdri.magic) {
        throwCantUnpack("file header corrupted");
    }
    unsigned const sz_cmds = mhdri.sizeofcmds;
    if ((sizeof(mhdri) + sz_cmds) > (size_t)fi->st_size()) {
        throwCantUnpack("file header corrupted");
    }
    rawmseg = (Mach_segment_command *) New(char, sz_cmds);
    fi->readx(rawmseg, mhdri.sizeofcmds);

    // FIXME forgot space left for LC_CODE_SIGNATURE;
    // but canUnpack() sets overlay_offset anyway.
    //overlay_offset = sizeof(mhdri) + mhdri.sizeofcmds + sizeof(linfo);

    fi->seek(overlay_offset, SEEK_SET);
    p_info hbuf;
    fi->readx(&hbuf, sizeof(hbuf));
    unsigned const orig_file_size = get_te32(&hbuf.p_filesize);
    blocksize = get_te32(&hbuf.p_blocksize);  // emacs-21.2.1 was 0x01d47e6c (== 30703212)
    if (blocksize > orig_file_size || blocksize > 0x05000000)
        throwCantUnpack("file header corrupted");
    if (file_size > (off_t)orig_file_size) {
        opt->info_mode += !opt->info_mode ? 1 : 0;  // make visible
        opt->backup = 1;
        infoWarning("packed size too big; discarding appended data, keeping backup");
    }

    ibuf.alloc(blocksize + OVERHEAD);
    b_info bhdr; memset(&bhdr, 0, sizeof(bhdr));
    fi->readx(&bhdr, sizeof(bhdr));
    ph.u_len = get_te32(&bhdr.sz_unc);
    ph.c_len = get_te32(&bhdr.sz_cpr);
    if ((unsigned)file_size < ph.c_len || ph.c_len == 0 || ph.u_len == 0)
        throwCantUnpack("file header corrupted");
    ph.method = bhdr.b_method;
    ph.filter = bhdr.b_ftid;
    ph.filter_cto = bhdr.b_cto8;

    // Uncompress Macho headers
    fi->readx(ibuf, ph.c_len);
    Mach_header *const mhdr = (Mach_header *) New(upx_byte, ph.u_len);
    decompress(ibuf, (upx_byte *)mhdr, false);
    if (mhdri.magic      != mhdr->magic
    ||  mhdri.cputype    != mhdr->cputype
    ||  mhdri.cpusubtype != mhdr->cpusubtype
    ||  mhdri.filetype   != mhdr->filetype)
        throwCantUnpack("file header corrupted");
    unsigned const ncmds = mhdr->ncmds;

    msegcmd = New(Mach_segment_command, ncmds);
    unsigned char const *ptr = (unsigned char const *)(1+mhdr);
    for (unsigned j= 0; j < ncmds; ++j) {
        memcpy(&msegcmd[j], ptr, umin(sizeof(Mach_segment_command),
            ((Mach_command const *)ptr)->cmdsize));
        ptr += (unsigned) ((Mach_segment_command const *)ptr)->cmdsize;
        if (ptr_udiff(ptr, mhdr) > ph.u_len) {
            throwCantUnpack("cmdsize");
        }
    }

    // Put LC_SEGMENT together at the beginning, ascending by .vmaddr.
    qsort(msegcmd, ncmds, sizeof(*msegcmd), compare_segment_command);
    n_segment = 0;
    for (unsigned j= 0; j < ncmds; ++j) {
        n_segment += (lc_seg==msegcmd[j].cmd);
    }

    unsigned total_in = 0;
    unsigned total_out = 0;
    unsigned c_adler = upx_adler32(NULL, 0);
    unsigned u_adler = upx_adler32(NULL, 0);

    fi->seek(- (off_t)(sizeof(bhdr) + ph.c_len), SEEK_CUR);
    for (unsigned k = 0; k < ncmds; ++k) {
        if (msegcmd[k].cmd==lc_seg && msegcmd[k].filesize!=0) {
            if (fo)
                fo->seek(msegcmd[k].fileoff, SEEK_SET);
            unpackExtent(msegcmd[k].filesize, fo, total_in, total_out,
                c_adler, u_adler, false, sizeof(bhdr));
            if (my_filetype==Mach_header::MH_DYLIB) {
                break;  // only the first lc_seg when MH_DYLIB
            }
        }
    }
    Mach_segment_command const *sc = (Mach_segment_command const *)(void *)(1+ mhdr);
    if (my_filetype==Mach_header::MH_DYLIB) { // rest of lc_seg are not compressed
        Mach_segment_command const *rc = rawmseg;
        rc = (Mach_segment_command const *)(rc->cmdsize + (char const *)rc);
        sc = (Mach_segment_command const *)(sc->cmdsize + (char const *)sc);
        for (
            unsigned k=1;  // skip first lc_seg, which was uncompressed above
            k < ncmds;
            (++k), (sc = (Mach_segment_command const *)(sc->cmdsize + (char const *)sc)),
                   (rc = (Mach_segment_command const *)(rc->cmdsize + (char const *)rc))
        ) {
            if (lc_seg==rc->cmd
            &&  0!=rc->filesize ) {
                fi->seek(rc->fileoff, SEEK_SET);
                if (fo)
                    fo->seek(sc->fileoff, SEEK_SET);
                unsigned const len = rc->filesize;
                MemBuffer data(len);
                fi->readx(data, len);
                if (fo)
                    fo->write(data, len);
            }
        }
    }
    else
    for (unsigned j = 0; j < ncmds; ++j) {
        unsigned const size = find_SEGMENT_gap(j, orig_file_size);
        if (size) {
            unsigned const where = msegcmd[j].fileoff +msegcmd[j].filesize;
            if (fo)
                fo->seek(where, SEEK_SET);
            unpackExtent(size, fo, total_in, total_out,
                c_adler, u_adler, false, sizeof(bhdr));
        }
    }
    delete [] mhdr;
}