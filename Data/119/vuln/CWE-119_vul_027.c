int Protocol2PacketHandler::readTxRx(PortHandler *port, uint8_t id, uint16_t address, uint16_t length, uint8_t *data, uint8_t *error)
{
  int result                  = COMM_TX_FAIL;

  uint8_t txpacket[14]        = {0};
  uint8_t *rxpacket           = (uint8_t *)malloc(length + 11 + (length / 3));
  //(length + 11 + (length/3));  // (length/3): consider stuffing

  if (rxpacket == NULL)
    return result;
  
  if (id >= BROADCAST_ID)
    return COMM_NOT_AVAILABLE;

  txpacket[PKT_ID]            = id;
  txpacket[PKT_LENGTH_L]      = 7;
  txpacket[PKT_LENGTH_H]      = 0;
  txpacket[PKT_INSTRUCTION]   = INST_READ;
  txpacket[PKT_PARAMETER0+0]  = (uint8_t)DXL_LOBYTE(address);
  txpacket[PKT_PARAMETER0+1]  = (uint8_t)DXL_HIBYTE(address);
  txpacket[PKT_PARAMETER0+2]  = (uint8_t)DXL_LOBYTE(length);
  txpacket[PKT_PARAMETER0+3]  = (uint8_t)DXL_HIBYTE(length);

  result = txRxPacket(port, txpacket, rxpacket, error);
  if (result == COMM_SUCCESS)
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