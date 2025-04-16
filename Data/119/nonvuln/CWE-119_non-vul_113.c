asn1_ctx_t *get_asn1_ctx(void *ptr) {
  asn1_ctx_t *actx = (asn1_ctx_t*)ptr;

  if (!asn1_ctx_check_signature(actx))
    actx = NULL;

  return actx;
}