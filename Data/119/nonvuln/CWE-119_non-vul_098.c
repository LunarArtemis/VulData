void asn1_stack_frame_check(asn1_ctx_t *actx, const gchar *name, const asn1_par_def_t *par_def) {
  const asn1_par_def_t *pd = par_def;
  asn1_par_t *par;

  DISSECTOR_ASSERT(actx->stack);
  DISSECTOR_ASSERT(!strcmp(actx->stack->name, name));

  par = actx->stack->par;
  while (pd->name) {
    DISSECTOR_ASSERT(par);
    DISSECTOR_ASSERT((pd->ptype == ASN1_PAR_IRR) || (par->ptype == pd->ptype));
    par->name = pd->name;
    pd++;
    par = par->next;
  }
  DISSECTOR_ASSERT(!par);
}