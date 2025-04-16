static struct mschmd_header *chmd_real_open(struct mschm_decompressor *base,
                                            const char *filename, int entire)
{
  struct mschm_decompressor_p *self = (struct mschm_decompressor_p *) base;
  struct mschmd_header *chm = NULL;
  struct mspack_system *sys;
  struct mspack_file *fh;
  int error;

  if (!base) return NULL;
  sys = self->system;

  if ((fh = sys->open(sys, filename, MSPACK_SYS_OPEN_READ))) {
    if ((chm = (struct mschmd_header *) sys->alloc(sys, sizeof(struct mschmd_header)))) {
      chm->filename = filename;
      error = chmd_read_headers(sys, fh, chm, entire);
      if (error) {
        /* if the error is DATAFORMAT, and there are some results, return
         * partial results with a warning, rather than nothing */
        if (error == MSPACK_ERR_DATAFORMAT && (chm->files || chm->sysfiles)) {
          sys->message(fh, "WARNING; contents are corrupt");
          error = MSPACK_ERR_OK;
        }
        else {
          chmd_close(base, chm);
          chm = NULL;
        }
      }
      self->error = error;
    }
    else {
      self->error = MSPACK_ERR_NOMEMORY;
    }
    sys->close(fh);
  }
  else {
    self->error = MSPACK_ERR_OPEN;
  }
  return chm;
}