static asn1_par_t *push_new_par(asn1_ctx_t *actx) {
  asn1_par_t *par, **pp;

  DISSECTOR_ASSERT(actx->stack);

  par = wmem_new0(wmem_packet_scope(), asn1_par_t);

  pp = &(actx->stack->par);
  while (*pp)
    pp = &((*pp)->next);
  *pp = par;

  return par;
}