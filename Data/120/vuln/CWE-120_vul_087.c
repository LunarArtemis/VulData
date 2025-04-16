int net_js_recv(JsNetwork *net, SocketType socketType, int sckt, void *buf, size_t len) {
  NOT_USED(net);
  JsVar *args[3] = {
      jsvNewFromInteger(sckt),
      jsvNewFromInteger((JsVarInt)len),
      jsvNewFromInteger((JsVarInt)socketType),
  };
  JsVar *res = callFn( "recv", 3, args);
  jsvUnLockMany(3, args);
  int r = -1; // fail
  if (jsvIsString(res)) {
    r = (int)jsvGetStringLength(res);
    if (r>(int)len) { r=(int)len; assert(0); }
    jsvGetStringChars(res, 0, (char*)buf, (size_t)r);
    // FIXME: jsvGetStringChars adds a 0 - does that actually write past the end of the array, or clip the data we get?
  } else if (jsvIsInt(res)) {
    r = jsvGetInteger(res);
    if (r>=0) {
      jsExceptionHere(JSET_ERROR, "JSNetwork.recv returned >=0");
      r=-1;
    }
  }
  jsvUnLock(res);
  return r;
}