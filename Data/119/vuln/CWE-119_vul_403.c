bool HasConcatSpreadableModifier(Isolate* isolate, Handle<JSArray> obj) {
  DCHECK(isolate->IsFastArrayConstructorPrototypeChainIntact());
  Handle<Symbol> key(isolate->factory()->is_concat_spreadable_symbol());
  Maybe<bool> maybe = JSReceiver::HasProperty(obj, key);
  return maybe.FromMaybe(false);
}