gboolean asn1_param_get_boolean(asn1_ctx_t *actx, const gchar *name) {
  asn1_par_t *par = NULL;

  par = get_par_by_name(actx, name);
  DISSECTOR_ASSERT(par);
  return par->value.v_boolean;
}