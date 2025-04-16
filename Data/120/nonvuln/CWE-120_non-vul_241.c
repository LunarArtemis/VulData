USBH_StatusTypeDef USBH_Get_CfgDesc(USBH_HandleTypeDef *phost,
                                    uint16_t length)

{
  USBH_StatusTypeDef status;
  uint8_t *pData = phost->device.CfgDesc_Raw;

  status = USBH_GetDescriptor(phost, (USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD),
                              USB_DESC_CONFIGURATION, pData, length);

  if (status == USBH_OK)
  {
    /* Commands successfully sent and Response Received  */
    status = USBH_ParseCfgDesc(phost, pData, length);
  }

  return status;
}