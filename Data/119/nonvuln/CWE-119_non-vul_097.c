rose_ctx_t *get_rose_ctx(void *ptr) {
  rose_ctx_t *rctx = (rose_ctx_t*)ptr;
  asn1_ctx_t *actx = (asn1_ctx_t*)ptr;

  if (!asn1_ctx_check_signature(actx))
    actx = NULL;

  if (actx)
    rctx = actx->rose_ctx;

  if (!rose_ctx_check_signature(rctx))
    rctx = NULL;

  return rctx;
}