void rose_ctx_init(rose_ctx_t *rctx) {
  memset(rctx, '\0', sizeof(*rctx));
  rctx->signature = ROSE_CTX_SIGNATURE;
}