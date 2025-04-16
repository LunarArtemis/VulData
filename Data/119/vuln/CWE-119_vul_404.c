inline bool IsJSArrayFastElementMovingAllowed(Isolate* isolate,
                                              JSArray* receiver) {
  DisallowHeapAllocation no_gc;
  // If the array prototype chain is intact (and free of elements), and if the
  // receiver's prototype is the array prototype, then we are done.
  Object* prototype = receiver->map()->prototype();
  if (prototype->IsJSArray() &&
      isolate->is_initial_array_prototype(JSArray::cast(prototype)) &&
      isolate->IsFastArrayConstructorPrototypeChainIntact()) {
    return true;
  }

  // Slow case.
  PrototypeIterator iter(isolate, receiver);
  return PrototypeHasNoElements(&iter);
}