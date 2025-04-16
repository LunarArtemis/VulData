USBH_StatusTypeDef USBH_ClrFeature(USBH_HandleTypeDef *phost, uint8_t ep_num)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_ENDPOINT
                                           | USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_CLEAR_FEATURE;
    phost->Control.setup.b.wValue.w = FEATURE_SELECTOR_ENDPOINT;
    phost->Control.setup.b.wIndex.w = ep_num;
    phost->Control.setup.b.wLength.w = 0U;
  }
  return USBH_CtlReq(phost, NULL, 0U);
}