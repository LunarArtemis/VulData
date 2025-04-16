static void chmd_close(struct mschm_decompressor *base,
                       struct mschmd_header *chm)
{
  struct mschm_decompressor_p *self = (struct mschm_decompressor_p *) base;
  struct mschmd_file *fi, *nfi;
  struct mspack_system *sys;
  unsigned int i;

  if (!base) return;
  sys = self->system;

  self->error = MSPACK_ERR_OK;

  /* free files */
  for (fi = chm->files; fi; fi = nfi) {
    nfi = fi->next;
    sys->free(fi);
  }
  for (fi = chm->sysfiles; fi; fi = nfi) {
    nfi = fi->next;
    sys->free(fi);
  }

  /* if this CHM was being decompressed, free decompression state */
  if (self->d && (self->d->chm == chm)) {
    if (self->d->infh) sys->close(self->d->infh);
    if (self->d->state) lzxd_free(self->d->state);
    sys->free(self->d);
    self->d = NULL;
  }

  /* if this CHM had a chunk cache, free it and contents */
  if (chm->chunk_cache) {
      for (i = 0; i < chm->num_chunks; i++) sys->free(chm->chunk_cache[i]);
      sys->free(chm->chunk_cache);
  }

  sys->free(chm);
}