void asn1_stack_frame_push(asn1_ctx_t *actx, const gchar *name) {
  asn1_stack_frame_t *frame;

  frame = wmem_new0(wmem_packet_scope(), asn1_stack_frame_t);
  frame->name = name;
  frame->next = actx->stack;
  actx->stack = frame;
}