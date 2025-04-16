static unsigned char *read_sys_file(struct mschm_decompressor_p *self,
                                    struct mschmd_file *file)
{
  struct mspack_system *sys = self->system;
  unsigned char *data = NULL;
  int len;

  if (!file || !file->section || (file->section->id != 0)) {
    self->error = MSPACK_ERR_DATAFORMAT;
    return NULL;
  }

  len = (int) file->length;

  if (!(data = (unsigned char *) sys->alloc(sys, (size_t) len))) {
    self->error = MSPACK_ERR_NOMEMORY;
    return NULL;
  }
  if (sys->seek(self->d->infh, file->section->chm->sec0.offset
                + file->offset, MSPACK_SYS_SEEK_START))
  {
    self->error = MSPACK_ERR_SEEK;
    sys->free(data);
    return NULL;
  }
  if (sys->read(self->d->infh, data, len) != len) {
    self->error = MSPACK_ERR_READ;
    sys->free(data);
    return NULL;
  }
  return data;
}