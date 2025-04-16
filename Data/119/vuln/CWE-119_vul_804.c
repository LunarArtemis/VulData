void LoadIC::UpdateCaches(LookupIterator* lookup) {
  if (state() == UNINITIALIZED) {
    // This is the first time we execute this inline cache. Set the target to
    // the pre monomorphic stub to delay setting the monomorphic state.
    if (UseVector()) {
      ConfigureVectorState(PREMONOMORPHIC);
    } else {
      set_target(*pre_monomorphic_stub());
    }
    TRACE_IC("LoadIC", lookup->name());
    return;
  }

  Handle<Code> code;
  if (lookup->state() == LookupIterator::JSPROXY ||
      lookup->state() == LookupIterator::ACCESS_CHECK) {
    code = slow_stub();
  } else if (!lookup->IsFound()) {
    if (kind() == Code::LOAD_IC) {
      code = NamedLoadHandlerCompiler::ComputeLoadNonexistent(lookup->name(),
                                                              receiver_map());
      // TODO(jkummerow/verwaest): Introduce a builtin that handles this case.
      if (code.is_null()) code = slow_stub();
    } else {
      code = slow_stub();
    }
  } else {
    code = ComputeHandler(lookup);
  }

  PatchCache(lookup->name(), code);
  TRACE_IC("LoadIC", lookup->name());
}