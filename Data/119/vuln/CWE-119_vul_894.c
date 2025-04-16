void ScavengeVisitor::ScavengePointer(Object** p) {
  Object* object = *p;
  if (!heap_->InNewSpace(object)) return;

  if (heap_->PurgeLeftTrimmedObject(p)) return;

  Scavenger::ScavengeObject(reinterpret_cast<HeapObject**>(p),
                            reinterpret_cast<HeapObject*>(object));
}