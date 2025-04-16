void asn1_stack_frame_pop(asn1_ctx_t *actx, const gchar *name) {
  DISSECTOR_ASSERT(actx->stack);
  DISSECTOR_ASSERT(!strcmp(actx->stack->name, name));
  actx->stack = actx->stack->next;
}