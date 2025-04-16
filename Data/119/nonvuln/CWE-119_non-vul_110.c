void rose_ctx_clean_data(rose_ctx_t *rctx) {
  memset(&rctx->d, '\0', sizeof(rctx->d));
  rctx->d.code = -1;
}