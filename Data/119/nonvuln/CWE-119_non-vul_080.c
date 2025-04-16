static int read_reset_table(struct mschm_decompressor_p *self,
                            struct mschmd_sec_mscompressed *sec,
                            int entry, off_t *length_ptr, off_t *offset_ptr)
{
    struct mspack_system *sys = self->system;
    unsigned char *data;
    unsigned int pos, entrysize;

    /* do we have a ResetTable file? */
    int err = find_sys_file(self, sec, &sec->rtable, rtable_name);
    if (err) return 0;

    /* read ResetTable file */
    if (sec->rtable->length < lzxrt_headerSIZEOF) {
        D(("ResetTable file is too short"))
        return 0;
    }
    if (!(data = read_sys_file(self, sec->rtable))) {
        D(("can't read reset table"))
        return 0;
    }

    /* check sanity of reset table */
    if (EndGetI32(&data[lzxrt_FrameLen]) != LZX_FRAME_SIZE) {
        D(("bad reset table frame length"))
        sys->free(data);
        return 0;
    }

    /* get the uncompressed length of the LZX stream */
    if (read_off64(length_ptr, &data[lzxrt_UncompLen], sys, self->d->infh)) {
        sys->free(data);
        return 0;
    }

    entrysize = EndGetI32(&data[lzxrt_EntrySize]);
    pos = EndGetI32(&data[lzxrt_TableOffset]) + (entry * entrysize);

    /* ensure reset table entry for this offset exists */
    if (entry < EndGetI32(&data[lzxrt_NumEntries]) &&
        pos <= (sec->rtable->length - entrysize))
    {
        switch (entrysize) {
        case 4:
            *offset_ptr = EndGetI32(&data[pos]);
            err = 0;
            break;
        case 8:
            err = read_off64(offset_ptr, &data[pos], sys, self->d->infh);
            break;
        default:
            D(("reset table entry size neither 4 nor 8"))
            err = 1;
            break;
        }
    }
    else {
        D(("bad reset interval"))
        err = 1;
    }

    /* free the reset table */
    sys->free(data);

    /* return success */
    return (err == 0);
}