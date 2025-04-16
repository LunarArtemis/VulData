static USBH_StatusTypeDef  USBH_ParseEPDesc(USBH_HandleTypeDef *phost, USBH_EpDescTypeDef  *ep_descriptor,
                                            uint8_t *buf)
{
  USBH_StatusTypeDef status = USBH_OK;
  ep_descriptor->bLength          = *(uint8_t *)(buf + 0);
  ep_descriptor->bDescriptorType  = *(uint8_t *)(buf + 1);
  ep_descriptor->bEndpointAddress = *(uint8_t *)(buf + 2);
  ep_descriptor->bmAttributes     = *(uint8_t *)(buf + 3);
  ep_descriptor->wMaxPacketSize   = LE16(buf + 4);
  ep_descriptor->bInterval        = *(uint8_t *)(buf + 6);

  /* Make sure that wMaxPacketSize is different from 0 */
  if (ep_descriptor->wMaxPacketSize == 0x00U)
  {
    status = USBH_NOT_SUPPORTED;
  }
  else if (USBH_MAX_EP_PACKET_SIZE < (uint16_t)USBH_MAX_DATA_BUFFER)
  {
    /* Make sure that maximum packet size (bits 0..10) does not exceed the max endpoint packet size */
    ep_descriptor->wMaxPacketSize &= ~0x7FFU;
    ep_descriptor->wMaxPacketSize |=  MIN((uint16_t)(LE16(buf + 4) & 0x7FFU), (uint16_t)USBH_MAX_EP_PACKET_SIZE);

  }
  else if ((uint16_t)USBH_MAX_DATA_BUFFER < USBH_MAX_EP_PACKET_SIZE)
  {
    /* Make sure that maximum packet size (bits 0..10) does not exceed the total buffer length */
    ep_descriptor->wMaxPacketSize &= ~0x7FFU;
    ep_descriptor->wMaxPacketSize |= MIN((uint16_t)(LE16(buf + 4) & 0x7FFU), (uint16_t)USBH_MAX_DATA_BUFFER);
  }
  else
  {
    /* ... */
  }

  /* For high-speed interrupt/isochronous endpoints, bInterval can vary from 1 to 16 */
  if (phost->device.speed == (uint8_t)USBH_SPEED_HIGH)
  {
    if (((ep_descriptor->bmAttributes & EP_TYPE_MSK) == EP_TYPE_ISOC) ||
        ((ep_descriptor->bmAttributes & EP_TYPE_MSK) == EP_TYPE_INTR))
    {
      if ((ep_descriptor->bInterval == 0U) || (ep_descriptor->bInterval > 0x10U))
      {
        status = USBH_NOT_SUPPORTED;
      }
    }
  }
  else
  {
    /* For full-speed isochronous endpoints, the value of bInterval must be in the range from 1 to 16.*/
    if ((ep_descriptor->bmAttributes & EP_TYPE_MSK) == EP_TYPE_ISOC)
    {
      if ((ep_descriptor->bInterval == 0U) || (ep_descriptor->bInterval > 0x10U))
      {
        status = USBH_NOT_SUPPORTED;
      }
    }
    /* For full-/low-speed interrupt endpoints, the value of bInterval may be from 1 to 255.*/
    else if ((ep_descriptor->bmAttributes & EP_TYPE_MSK) == EP_TYPE_INTR)
    {
      if (ep_descriptor->bInterval == 0U)
      {
        status = USBH_NOT_SUPPORTED;
      }
    }
    else
    {
      /* ... */
    }
  }

  return status;
}