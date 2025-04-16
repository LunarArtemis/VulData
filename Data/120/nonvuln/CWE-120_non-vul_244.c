USBH_StatusTypeDef USBH_SetAddress(USBH_HandleTypeDef *phost,
                                   uint8_t DeviceAddress)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | \
                                           USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_SET_ADDRESS;

    phost->Control.setup.b.wValue.w = (uint16_t)DeviceAddress;
    phost->Control.setup.b.wIndex.w = 0U;
    phost->Control.setup.b.wLength.w = 0U;
  }

  return USBH_CtlReq(phost, NULL, 0U);
}