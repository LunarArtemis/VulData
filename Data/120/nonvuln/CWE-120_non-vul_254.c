USBH_StatusTypeDef USBH_SetCfg(USBH_HandleTypeDef *phost, uint16_t cfg_idx)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE
                                           | USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_SET_CONFIGURATION;
    phost->Control.setup.b.wValue.w = cfg_idx;
    phost->Control.setup.b.wIndex.w = 0U;
    phost->Control.setup.b.wLength.w = 0U;
  }

  return USBH_CtlReq(phost, NULL, 0U);
}