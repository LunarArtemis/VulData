USBH_StatusTypeDef USBH_Get_DevDesc(USBH_HandleTypeDef *phost, uint8_t length)
{
  USBH_StatusTypeDef status;

  status = USBH_GetDescriptor(phost,
                              USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                              USB_DESC_DEVICE, phost->device.Data,
                              (uint16_t)length);

  if (status == USBH_OK)
  {
    /* Commands successfully sent and Response Received */
    USBH_ParseDevDesc(&phost->device.DevDesc, phost->device.Data,
                      (uint16_t)length);
  }

  return status;
}