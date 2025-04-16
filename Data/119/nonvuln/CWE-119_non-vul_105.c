void asn1_ctx_init(asn1_ctx_t *actx, asn1_enc_e encoding, gboolean aligned, packet_info *pinfo) {
  memset(actx, '\0', sizeof(*actx));
  actx->signature = ASN1_CTX_SIGNATURE;
  actx->encoding = encoding;
  actx->aligned = aligned;
  actx->pinfo = pinfo;
}