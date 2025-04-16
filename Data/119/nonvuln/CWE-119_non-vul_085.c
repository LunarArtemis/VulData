static int read_off64(off_t *var, unsigned char *mem,
                      struct mspack_system *sys, struct mspack_file *fh)
{
#if LARGEFILE_SUPPORT
    *var = EndGetI64(mem);
#else
    *var = EndGetI32(mem);
    if ((*var & 0x80000000) || EndGetI32(mem+4)) {
        sys->message(fh, (char *)largefile_msg);
        return 1;
    }
#endif
    return 0;
}