static asn1_par_t *get_par_by_name(asn1_ctx_t *actx, const gchar *name) {
  asn1_par_t *par = NULL;

  DISSECTOR_ASSERT(actx->stack);
  par = actx->stack->par;
  while (par) {
    if (!strcmp(par->name, name))
      return par;
    par = par->next;
  }
  return par;
}