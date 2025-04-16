void Heap::IterateStrongRoots(ObjectVisitor* v, VisitMode mode) {
  v->VisitPointers(&roots_[0], &roots_[kStrongRootListLength]);
  v->Synchronize(VisitorSynchronization::kStrongRootList);
  // The serializer/deserializer iterates the root list twice, first to pick
  // off immortal immovable roots to make sure they end up on the first page,
  // and then again for the rest.
  if (mode == VISIT_ONLY_STRONG_ROOT_LIST) return;

  isolate_->bootstrapper()->Iterate(v);
  v->Synchronize(VisitorSynchronization::kBootstrapper);
  isolate_->Iterate(v);
  v->Synchronize(VisitorSynchronization::kTop);
  Relocatable::Iterate(isolate_, v);
  v->Synchronize(VisitorSynchronization::kRelocatable);
  isolate_->debug()->Iterate(v);
  v->Synchronize(VisitorSynchronization::kDebug);

  isolate_->compilation_cache()->Iterate(v);
  v->Synchronize(VisitorSynchronization::kCompilationCache);

  // Iterate over local handles in handle scopes.
  isolate_->handle_scope_implementer()->Iterate(v);
  isolate_->IterateDeferredHandles(v);
  v->Synchronize(VisitorSynchronization::kHandleScope);

  // Iterate over the builtin code objects and code stubs in the
  // heap. Note that it is not necessary to iterate over code objects
  // on scavenge collections.
  if (mode != VISIT_ALL_IN_SCAVENGE) {
    isolate_->builtins()->IterateBuiltins(v);
    v->Synchronize(VisitorSynchronization::kBuiltins);
    isolate_->interpreter()->IterateDispatchTable(v);
    v->Synchronize(VisitorSynchronization::kDispatchTable);
  }

  // Iterate over global handles.
  switch (mode) {
    case VISIT_ONLY_STRONG_ROOT_LIST:
      UNREACHABLE();
      break;
    case VISIT_ONLY_STRONG:
    case VISIT_ONLY_STRONG_FOR_SERIALIZATION:
      isolate_->global_handles()->IterateStrongRoots(v);
      break;
    case VISIT_ALL_IN_SCAVENGE:
      isolate_->global_handles()->IterateNewSpaceStrongAndDependentRoots(v);
      break;
    case VISIT_ALL_IN_SWEEP_NEWSPACE:
    case VISIT_ALL:
      isolate_->global_handles()->IterateAllRoots(v);
      break;
  }
  v->Synchronize(VisitorSynchronization::kGlobalHandles);

  // Iterate over eternal handles.
  if (mode == VISIT_ALL_IN_SCAVENGE) {
    isolate_->eternal_handles()->IterateNewSpaceRoots(v);
  } else {
    isolate_->eternal_handles()->IterateAllRoots(v);
  }
  v->Synchronize(VisitorSynchronization::kEternalHandles);

  // Iterate over pointers being held by inactive threads.
  isolate_->thread_manager()->Iterate(v);
  v->Synchronize(VisitorSynchronization::kThreadManager);

  // Iterate over other strong roots (currently only identity maps).
  for (StrongRootsList* list = strong_roots_list_; list; list = list->next) {
    v->VisitPointers(list->start, list->end);
  }
  v->Synchronize(VisitorSynchronization::kStrongRoots);

  // Iterate over the partial snapshot cache unless serializing.
  if (mode != VISIT_ONLY_STRONG_FOR_SERIALIZATION) {
    SerializerDeserializer::Iterate(isolate_, v);
  }
  // We don't do a v->Synchronize call here, because in debug mode that will
  // output a flag to the snapshot.  However at this point the serializer and
  // deserializer are deliberately a little unsynchronized (see above) so the
  // checking of the sync flag in the snapshot would fail.
}