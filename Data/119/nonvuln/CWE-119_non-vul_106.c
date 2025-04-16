gboolean rose_ctx_check_signature(rose_ctx_t *rctx) {
  return rctx && (rctx->signature == ROSE_CTX_SIGNATURE);
}