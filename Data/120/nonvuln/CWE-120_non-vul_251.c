USBH_StatusTypeDef USBH_Get_StringDesc(USBH_HandleTypeDef *phost,
                                       uint8_t string_index, uint8_t *buff,
                                       uint16_t length)
{
  USBH_StatusTypeDef status;

  status = USBH_GetDescriptor(phost,
                              USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                              USB_DESC_STRING | string_index,
                              phost->device.Data, length);

  if (status == USBH_OK)
  {
    /* Commands successfully sent and Response Received  */
    USBH_ParseStringDesc(phost->device.Data, buff, length);
  }

  return status;
}