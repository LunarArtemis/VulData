MaybeHandle<JSArray> Fast_ArrayConcat(Isolate* isolate, Arguments* args) {
  if (!isolate->IsFastArrayConstructorPrototypeChainIntact()) {
    return MaybeHandle<JSArray>();
  }
  int n_arguments = args->length();
  int result_len = 0;
  {
    DisallowHeapAllocation no_gc;
    Object* array_proto = isolate->array_function()->prototype();
    // Iterate through all the arguments performing checks
    // and calculating total length.
    for (int i = 0; i < n_arguments; i++) {
      Object* arg = (*args)[i];
      if (!arg->IsJSArray()) return MaybeHandle<JSArray>();
      Handle<JSArray> array(JSArray::cast(arg), isolate);
      if (!array->HasFastElements()) return MaybeHandle<JSArray>();
      PrototypeIterator iter(isolate, *array);
      if (iter.GetCurrent() != array_proto) return MaybeHandle<JSArray>();
      if (HasConcatSpreadableModifier(isolate, array)) {
        return MaybeHandle<JSArray>();
      }
      int len = Smi::cast(array->length())->value();

      // We shouldn't overflow when adding another len.
      const int kHalfOfMaxInt = 1 << (kBitsPerInt - 2);
      STATIC_ASSERT(FixedArray::kMaxLength < kHalfOfMaxInt);
      USE(kHalfOfMaxInt);
      result_len += len;
      DCHECK(result_len >= 0);
      // Throw an Error if we overflow the FixedArray limits
      if (FixedArray::kMaxLength < result_len) {
        THROW_NEW_ERROR(isolate,
                        NewRangeError(MessageTemplate::kInvalidArrayLength),
                        JSArray);
      }
    }
  }
  return ElementsAccessor::Concat(isolate, args, n_arguments);
}