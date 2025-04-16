MUST_USE_RESULT
inline bool EnsureJSArrayWithWritableFastElements(Isolate* isolate,
                                                  Handle<Object> receiver,
                                                  Arguments* args,
                                                  int first_added_arg) {
  if (!receiver->IsJSArray()) return false;
  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  // If there may be elements accessors in the prototype chain, the fast path
  // cannot be used if there arguments to add to the array.
  if (args != nullptr && !IsJSArrayFastElementMovingAllowed(isolate, *array)) {
    return false;
  }
  ElementsKind origin_kind = array->map()->elements_kind();
  if (IsDictionaryElementsKind(origin_kind)) return false;
  if (array->map()->is_observed()) return false;
  if (!array->map()->is_extensible()) return false;
  if (args == nullptr) return true;

  // Adding elements to the array prototype would break code that makes sure
  // it has no elements. Handle that elsewhere.
  if (isolate->IsAnyInitialArrayPrototype(array)) return false;

  // Need to ensure that the arguments passed in args can be contained in
  // the array.
  int args_length = args->length();
  if (first_added_arg >= args_length) return true;

  if (IsFastObjectElementsKind(origin_kind)) return true;
  ElementsKind target_kind = origin_kind;
  {
    DisallowHeapAllocation no_gc;
    int arg_count = args_length - first_added_arg;
    Object** arguments = args->arguments() - first_added_arg - (arg_count - 1);
    for (int i = 0; i < arg_count; i++) {
      Object* arg = arguments[i];
      if (arg->IsHeapObject()) {
        if (arg->IsHeapNumber()) {
          target_kind = FAST_DOUBLE_ELEMENTS;
        } else {
          target_kind = FAST_ELEMENTS;
          break;
        }
      }
    }
  }
  if (target_kind != origin_kind) {
    // Use a short-lived HandleScope to avoid creating several copies of the
    // elements handle which would cause issues when left-trimming later-on.
    HandleScope scope(isolate);
    JSObject::TransitionElementsKind(array, target_kind);
  }
  return true;
}