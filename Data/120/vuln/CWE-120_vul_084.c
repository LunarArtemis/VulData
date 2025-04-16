void socketReceivedUDP(JsVar *connection, JsVar **receiveData) {
  // Get the header
  size_t len = jsvGetStringLength(*receiveData);
  if (len < sizeof(JsNetUDPPacketHeader)) return; // not enough data for header!
  char buf[sizeof(JsNetUDPPacketHeader)+1]; // trailing 0 from jsvGetStringChars
  jsvGetStringChars(*receiveData, 0, buf, sizeof(JsNetUDPPacketHeader)+1);
  JsNetUDPPacketHeader *header = (JsNetUDPPacketHeader*)buf;
  if (sizeof(JsNetUDPPacketHeader)+header->length < len) return; // not enough data yet

  JsVar *rinfo = jsvNewObject();
  if (rinfo) {
    // split the received data string to get the data we need
    JsVar *data = jsvNewFromStringVar(*receiveData, sizeof(JsNetUDPPacketHeader), header->length);
    JsVar *newReceiveData = 0;
    if (len > sizeof(JsNetUDPPacketHeader)+header->length)
      newReceiveData = jsvNewFromStringVar(*receiveData, sizeof(JsNetUDPPacketHeader)+header->length, JSVAPPENDSTRINGVAR_MAXLENGTH);
    jsvUnLock(*receiveData);
    *receiveData = newReceiveData;
    // fire the received data event
    jsvObjectSetChildAndUnLock(rinfo, "address", jsvVarPrintf("%d.%d.%d.%d", header->host[0], header->host[1], header->host[2], header->host[3]));
    jsvObjectSetChildAndUnLock(rinfo, "port", jsvNewFromInteger(header->port));
    jsvObjectSetChildAndUnLock(rinfo, "size", jsvNewFromInteger(header->length));
    JsVar *args[2] = { data, rinfo };
    jsiQueueObjectCallbacks(connection, DGRAM_NAME_ON_MESSAGE, args, 2);
    jsvUnLock2(data,rinfo);
  }
}