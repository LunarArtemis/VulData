void asn1_ctx_clean_external(asn1_ctx_t *actx) {
  memset(&actx->external, '\0', sizeof(actx->external));
  actx->external.hf_index = -1;
  actx->external.encoding = -1;
}