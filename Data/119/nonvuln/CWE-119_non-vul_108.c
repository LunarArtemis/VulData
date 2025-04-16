void asn1_ctx_clean_epdv(asn1_ctx_t *actx) {
  memset(&actx->embedded_pdv, '\0', sizeof(actx->embedded_pdv));
  actx->embedded_pdv.hf_index = -1;
  actx->embedded_pdv.identification = -1;
}