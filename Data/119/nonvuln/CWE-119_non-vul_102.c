void asn1_param_push_integer(asn1_ctx_t *actx, gint32 value) {
  asn1_par_t *par;

  par = push_new_par(actx);
  par->ptype = ASN1_PAR_INTEGER;
  par->value.v_integer = value;
}