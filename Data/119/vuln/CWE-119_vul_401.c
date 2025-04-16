inline bool PrototypeHasNoElements(PrototypeIterator* iter) {
  DisallowHeapAllocation no_gc;
  for (; !iter->IsAtEnd(); iter->Advance()) {
    if (iter->GetCurrent()->IsJSProxy()) return false;
    JSObject* current = iter->GetCurrent<JSObject>();
    if (current->IsAccessCheckNeeded()) return false;
    if (current->HasIndexedInterceptor()) return false;
    if (current->elements()->length() != 0) return false;
  }
  return true;
}