Handle<Code> LoadIC::CompileHandler(LookupIterator* lookup,
                                    Handle<Object> unused,
                                    CacheHolderFlag cache_holder) {
  Handle<Object> receiver = lookup->GetReceiver();
  if (receiver->IsString() &&
      Name::Equals(isolate()->factory()->length_string(), lookup->name())) {
    FieldIndex index = FieldIndex::ForInObjectOffset(String::kLengthOffset);
    return SimpleFieldLoad(index);
  }

  if (receiver->IsStringWrapper() &&
      Name::Equals(isolate()->factory()->length_string(), lookup->name())) {
    StringLengthStub string_length_stub(isolate());
    return string_length_stub.GetCode();
  }

  // Use specialized code for getting prototype of functions.
  if (receiver->IsJSFunction() &&
      Name::Equals(isolate()->factory()->prototype_string(), lookup->name()) &&
      Handle<JSFunction>::cast(receiver)->should_have_prototype() &&
      !Handle<JSFunction>::cast(receiver)
           ->map()
           ->has_non_instance_prototype()) {
    Handle<Code> stub;
    FunctionPrototypeStub function_prototype_stub(isolate());
    return function_prototype_stub.GetCode();
  }

  Handle<Map> map = receiver_map();
  Handle<JSObject> holder = lookup->GetHolder<JSObject>();
  bool receiver_is_holder = receiver.is_identical_to(holder);
  switch (lookup->state()) {
    case LookupIterator::INTERCEPTOR: {
      DCHECK(!holder->GetNamedInterceptor()->getter()->IsUndefined());
      NamedLoadHandlerCompiler compiler(isolate(), map, holder, cache_holder);
      // Perform a lookup behind the interceptor. Copy the LookupIterator since
      // the original iterator will be used to fetch the value.
      LookupIterator it = *lookup;
      it.Next();
      LookupForRead(&it);
      return compiler.CompileLoadInterceptor(&it);
    }

    case LookupIterator::ACCESSOR: {
      // Use simple field loads for some well-known callback properties.
      // The method will only return true for absolute truths based on the
      // receiver maps.
      int object_offset;
      if (Accessors::IsJSObjectFieldAccessor(map, lookup->name(),
                                             &object_offset)) {
        FieldIndex index = FieldIndex::ForInObjectOffset(object_offset, *map);
        return SimpleFieldLoad(index);
      }
      if (Accessors::IsJSArrayBufferViewFieldAccessor(map, lookup->name(),
                                                      &object_offset)) {
        FieldIndex index = FieldIndex::ForInObjectOffset(object_offset, *map);
        ArrayBufferViewLoadFieldStub stub(isolate(), index);
        return stub.GetCode();
      }

      Handle<Object> accessors = lookup->GetAccessors();
      if (accessors->IsExecutableAccessorInfo()) {
        Handle<ExecutableAccessorInfo> info =
            Handle<ExecutableAccessorInfo>::cast(accessors);
        if (v8::ToCData<Address>(info->getter()) == 0) break;
        if (!ExecutableAccessorInfo::IsCompatibleReceiverMap(isolate(), info,
                                                             map)) {
          break;
        }
        if (!holder->HasFastProperties()) break;
        NamedLoadHandlerCompiler compiler(isolate(), map, holder, cache_holder);
        return compiler.CompileLoadCallback(lookup->name(), info);
      }
      if (accessors->IsAccessorPair()) {
        Handle<Object> getter(Handle<AccessorPair>::cast(accessors)->getter(),
                              isolate());
        if (!getter->IsJSFunction()) break;
        if (!holder->HasFastProperties()) break;
        Handle<JSFunction> function = Handle<JSFunction>::cast(getter);
        if (!receiver->IsJSObject() && !function->IsBuiltin() &&
            is_sloppy(function->shared()->language_mode())) {
          // Calling sloppy non-builtins with a value as the receiver
          // requires boxing.
          break;
        }
        CallOptimization call_optimization(function);
        NamedLoadHandlerCompiler compiler(isolate(), map, holder, cache_holder);
        if (call_optimization.is_simple_api_call() &&
            call_optimization.IsCompatibleReceiver(receiver, holder)) {
          return compiler.CompileLoadCallback(lookup->name(), call_optimization,
                                              lookup->GetAccessorIndex());
        }
        int expected_arguments =
            function->shared()->internal_formal_parameter_count();
        return compiler.CompileLoadViaGetter(
            lookup->name(), lookup->GetAccessorIndex(), expected_arguments);
      }
      break;
    }

    case LookupIterator::DATA: {
      if (lookup->is_dictionary_holder()) {
        if (kind() != Code::LOAD_IC) break;
        if (holder->IsGlobalObject()) {
          NamedLoadHandlerCompiler compiler(isolate(), map, holder,
                                            cache_holder);
          Handle<PropertyCell> cell = lookup->GetPropertyCell();
          Handle<Code> code = compiler.CompileLoadGlobal(
              cell, lookup->name(), lookup->IsConfigurable());
          // TODO(verwaest): Move caching of these NORMAL stubs outside as well.
          CacheHolderFlag flag;
          Handle<Map> stub_holder_map =
              GetHandlerCacheHolder(map, receiver_is_holder, isolate(), &flag);
          Map::UpdateCodeCache(stub_holder_map, lookup->name(), code);
          return code;
        }
        // There is only one shared stub for loading normalized
        // properties. It does not traverse the prototype chain, so the
        // property must be found in the object for the stub to be
        // applicable.
        if (!receiver_is_holder) break;
        return isolate()->builtins()->LoadIC_Normal();
      }

      // -------------- Fields --------------
      if (lookup->property_details().type() == DATA) {
        FieldIndex field = lookup->GetFieldIndex();
        if (receiver_is_holder) {
          return SimpleFieldLoad(field);
        }
        NamedLoadHandlerCompiler compiler(isolate(), map, holder, cache_holder);
        return compiler.CompileLoadField(lookup->name(), field);
      }

      // -------------- Constant properties --------------
      DCHECK(lookup->property_details().type() == DATA_CONSTANT);
      if (receiver_is_holder) {
        LoadConstantStub stub(isolate(), lookup->GetConstantIndex());
        return stub.GetCode();
      }
      NamedLoadHandlerCompiler compiler(isolate(), map, holder, cache_holder);
      return compiler.CompileLoadConstant(lookup->name(),
                                          lookup->GetConstantIndex());
    }

    case LookupIterator::INTEGER_INDEXED_EXOTIC:
      return slow_stub();
    case LookupIterator::ACCESS_CHECK:
    case LookupIterator::JSPROXY:
    case LookupIterator::NOT_FOUND:
    case LookupIterator::TRANSITION:
      UNREACHABLE();
  }

  return slow_stub();
}