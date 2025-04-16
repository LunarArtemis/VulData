size_t jsvGetStringChars(const JsVar *v, size_t startChar, char *str, size_t len) {
  assert(jsvHasCharacterData(v));
  size_t l = len;
  JsvStringIterator it;
  jsvStringIteratorNewConst(&it, v, startChar);
  while (jsvStringIteratorHasChar(&it)) {
    if (l--<=0) {
      jsvStringIteratorFree(&it);
      return len;
    }
    *(str++) = jsvStringIteratorGetChar(&it);
    jsvStringIteratorNext(&it);
  }
  jsvStringIteratorFree(&it);
  *str = 0;
  return len-l;
}