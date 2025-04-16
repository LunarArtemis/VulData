USBH_StatusTypeDef USBH_GetDescriptor(USBH_HandleTypeDef *phost,
                                      uint8_t  req_type,
                                      uint16_t value_idx,
                                      uint8_t *buff,
                                      uint16_t length)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_D2H | req_type;
    phost->Control.setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
    phost->Control.setup.b.wValue.w = value_idx;

    if ((value_idx & 0xff00U) == USB_DESC_STRING)
    {
      phost->Control.setup.b.wIndex.w = 0x0409U;
    }
    else
    {
      phost->Control.setup.b.wIndex.w = 0U;
    }
    phost->Control.setup.b.wLength.w = length;
  }

  return USBH_CtlReq(phost, buff, length);
}