bool IterateElements(Isolate* isolate, Handle<JSReceiver> receiver,
                     ArrayConcatVisitor* visitor) {
  uint32_t length = 0;

  if (receiver->IsJSArray()) {
    Handle<JSArray> array = Handle<JSArray>::cast(receiver);
    length = static_cast<uint32_t>(array->length()->Number());
  } else {
    Handle<Object> val;
    Handle<Object> key = isolate->factory()->length_string();
    ASSIGN_RETURN_ON_EXCEPTION_VALUE(
        isolate, val, Runtime::GetObjectProperty(isolate, receiver, key),
        false);
    ASSIGN_RETURN_ON_EXCEPTION_VALUE(isolate, val,
                                     Object::ToLength(isolate, val), false);
    // TODO(caitp): Support larger element indexes (up to 2^53-1).
    if (!val->ToUint32(&length)) {
      length = 0;
    }
  }

  if (!receiver->IsJSArray()) {
    // For classes which are not known to be safe to access via elements alone,
    // use the slow case.
    return IterateElementsSlow(isolate, receiver, length, visitor);
  }
  Handle<JSObject> array = Handle<JSObject>::cast(receiver);

  switch (array->GetElementsKind()) {
    case FAST_SMI_ELEMENTS:
    case FAST_ELEMENTS:
    case FAST_HOLEY_SMI_ELEMENTS:
    case FAST_HOLEY_ELEMENTS: {
      // Run through the elements FixedArray and use HasElement and GetElement
      // to check the prototype for missing elements.
      Handle<FixedArray> elements(FixedArray::cast(array->elements()));
      int fast_length = static_cast<int>(length);
      DCHECK(fast_length <= elements->length());
      for (int j = 0; j < fast_length; j++) {
        HandleScope loop_scope(isolate);
        Handle<Object> element_value(elements->get(j), isolate);
        if (!element_value->IsTheHole()) {
          if (!visitor->visit(j, element_value)) return false;
        } else {
          Maybe<bool> maybe = JSReceiver::HasElement(array, j);
          if (!maybe.IsJust()) return false;
          if (maybe.FromJust()) {
            // Call GetElement on array, not its prototype, or getters won't
            // have the correct receiver.
            ASSIGN_RETURN_ON_EXCEPTION_VALUE(
                isolate, element_value,
                JSReceiver::GetElement(isolate, array, j), false);
            if (!visitor->visit(j, element_value)) return false;
          }
        }
      }
      break;
    }
    case FAST_HOLEY_DOUBLE_ELEMENTS:
    case FAST_DOUBLE_ELEMENTS: {
      // Empty array is FixedArray but not FixedDoubleArray.
      if (length == 0) break;
      // Run through the elements FixedArray and use HasElement and GetElement
      // to check the prototype for missing elements.
      if (array->elements()->IsFixedArray()) {
        DCHECK(array->elements()->length() == 0);
        break;
      }
      Handle<FixedDoubleArray> elements(
          FixedDoubleArray::cast(array->elements()));
      int fast_length = static_cast<int>(length);
      DCHECK(fast_length <= elements->length());
      for (int j = 0; j < fast_length; j++) {
        HandleScope loop_scope(isolate);
        if (!elements->is_the_hole(j)) {
          double double_value = elements->get_scalar(j);
          Handle<Object> element_value =
              isolate->factory()->NewNumber(double_value);
          if (!visitor->visit(j, element_value)) return false;
        } else {
          Maybe<bool> maybe = JSReceiver::HasElement(array, j);
          if (!maybe.IsJust()) return false;
          if (maybe.FromJust()) {
            // Call GetElement on array, not its prototype, or getters won't
            // have the correct receiver.
            Handle<Object> element_value;
            ASSIGN_RETURN_ON_EXCEPTION_VALUE(
                isolate, element_value,
                JSReceiver::GetElement(isolate, array, j), false);
            if (!visitor->visit(j, element_value)) return false;
          }
        }
      }
      break;
    }

    case DICTIONARY_ELEMENTS: {
      // CollectElementIndices() can't be called when there's a JSProxy
      // on the prototype chain.
      for (PrototypeIterator iter(isolate, array); !iter.IsAtEnd();
           iter.Advance()) {
        if (PrototypeIterator::GetCurrent(iter)->IsJSProxy()) {
          return IterateElementsSlow(isolate, array, length, visitor);
        }
      }
      Handle<SeededNumberDictionary> dict(array->element_dictionary());
      List<uint32_t> indices(dict->Capacity() / 2);
      // Collect all indices in the object and the prototypes less
      // than length. This might introduce duplicates in the indices list.
      CollectElementIndices(array, length, &indices);
      indices.Sort(&compareUInt32);
      int j = 0;
      int n = indices.length();
      while (j < n) {
        HandleScope loop_scope(isolate);
        uint32_t index = indices[j];
        Handle<Object> element;
        ASSIGN_RETURN_ON_EXCEPTION_VALUE(
            isolate, element, JSReceiver::GetElement(isolate, array, index),
            false);
        if (!visitor->visit(index, element)) return false;
        // Skip to next different index (i.e., omit duplicates).
        do {
          j++;
        } while (j < n && indices[j] == index);
      }
      break;
    }
    case FAST_SLOPPY_ARGUMENTS_ELEMENTS:
    case SLOW_SLOPPY_ARGUMENTS_ELEMENTS: {
      for (uint32_t index = 0; index < length; index++) {
        HandleScope loop_scope(isolate);
        Handle<Object> element;
        ASSIGN_RETURN_ON_EXCEPTION_VALUE(
            isolate, element, JSReceiver::GetElement(isolate, array, index),
            false);
        if (!visitor->visit(index, element)) return false;
      }
      break;
    }
    case NO_ELEMENTS:
      break;
#define TYPED_ARRAY_CASE(Type, type, TYPE, ctype, size) case TYPE##_ELEMENTS:
      TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE
    case FAST_STRING_WRAPPER_ELEMENTS:
    case SLOW_STRING_WRAPPER_ELEMENTS:
      // |array| is guaranteed to be an array or typed array.
      UNREACHABLE();
      break;
  }
  visitor->increase_index_offset(length);
  return true;
}