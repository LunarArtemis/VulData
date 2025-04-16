bool networkGetFromVar(JsNetwork *net) {
  // Retrieve a reference to the JsVar that represents the network and save in the
  // JsNetwork C structure.
  net->networkVar = jsvObjectGetChild(execInfo.hiddenRoot, NETWORK_VAR_NAME, 0);

  // Validate that we have a network variable.
  if (!net->networkVar) {
#ifdef LINUX
    networkCreate(net, JSNETWORKTYPE_SOCKET);
    return net->networkVar != 0;
#else
    return false;
#endif
  }

  // Retrieve the data for the network var and save in the data property of the JsNetwork
  // structure.
  jsvGetString(net->networkVar, (char *)&net->data, sizeof(JsNetworkData)+1/*trailing zero*/);

  // Now we know which kind of network we are working with, invoke the corresponding initialization
  // function to set the callbacks for this network tyoe.
  switch (net->data.type) {
#if defined(USE_CC3000)
  case JSNETWORKTYPE_CC3000 : netSetCallbacks_cc3000(net); break;
#endif
#if defined(USE_WIZNET)
  case JSNETWORKTYPE_W5500 : netSetCallbacks_wiznet(net); break;
#endif
#if defined(USE_ESP8266)
  case JSNETWORKTYPE_ESP8266_BOARD : netSetCallbacks_esp8266_board(net); break;
#endif
#if defined(USE_ESP32)
  case JSNETWORKTYPE_ESP32 : netSetCallbacks_esp32(net); break;
#endif
#if defined(LINUX)
  case JSNETWORKTYPE_SOCKET : netSetCallbacks_linux(net); break;
#endif
#if defined(USE_NETWORK_JS)
  case JSNETWORKTYPE_JS : netSetCallbacks_js(net); break;
#endif
  default:
    jsExceptionHere(JSET_INTERNALERROR, "Unknown network device %d", net->data.type);
    networkFree(net);
    return false;
  }

  // Adjust for SO_RCVBUF
  if (net->data.recvBufferSize > net->chunkSize) {
    net->chunkSize = net->data.recvBufferSize;
  }

  // Save the current network as a global.
  networkCurrentStruct = net;
  return true;
}