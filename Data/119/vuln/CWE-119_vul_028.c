int Protocol2PacketHandler::readRx(PortHandler *port, uint8_t id, uint16_t length, uint8_t *data, uint8_t *error)
{
  int result                  = COMM_TX_FAIL;
  uint8_t *rxpacket           = (uint8_t *)malloc(length + 11 + (length / 3));
  //(length + 11 + (length/3));  // (length/3): consider stuffing
  
  if (rxpacket == NULL)
    return result;
  
  do {
    result = rxPacket(port, rxpacket);
  } while (result == COMM_SUCCESS && rxpacket[PKT_ID] != id);

  if (result == COMM_SUCCESS && rxpacket[PKT_ID] == id)
  {
    if (error != 0)
      *error = (uint8_t)rxpacket[PKT_ERROR];

    for (uint16_t s = 0; s < length; s++)
    {
      data[s] = rxpacket[PKT_PARAMETER0 + 1 + s];
    }
    //memcpy(data, &rxpacket[PKT_PARAMETER0+1], length);
  }

  free(rxpacket);
  //delete[] rxpacket;
  return result;
}