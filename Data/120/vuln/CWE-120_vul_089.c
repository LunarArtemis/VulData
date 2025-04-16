void jswrap_ble_BluetoothDevice_sendPasskey(JsVar *parent, JsVar *passkeyVar) {
  char passkey[BLE_GAP_PASSKEY_LEN+1];
  memset(passkey, 0, sizeof(passkey));
  jsvGetString(passkeyVar, passkey, sizeof(passkey));
  uint32_t err_code = jsble_central_send_passkey(passkey);
  jsble_check_error(err_code);
}