gboolean asn1_ctx_check_signature(asn1_ctx_t *actx) {
  return actx && (actx->signature == ASN1_CTX_SIGNATURE);
}