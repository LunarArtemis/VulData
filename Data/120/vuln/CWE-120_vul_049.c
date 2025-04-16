static void  USBH_ParseInterfaceDesc(USBH_InterfaceDescTypeDef *if_descriptor,
                                     uint8_t *buf)
{
  if_descriptor->bLength            = *(uint8_t *)(buf + 0);
  if_descriptor->bDescriptorType    = *(uint8_t *)(buf + 1);
  if_descriptor->bInterfaceNumber   = *(uint8_t *)(buf + 2);
  if_descriptor->bAlternateSetting  = *(uint8_t *)(buf + 3);
  if_descriptor->bNumEndpoints      = *(uint8_t *)(buf + 4);
  if_descriptor->bInterfaceClass    = *(uint8_t *)(buf + 5);
  if_descriptor->bInterfaceSubClass = *(uint8_t *)(buf + 6);
  if_descriptor->bInterfaceProtocol = *(uint8_t *)(buf + 7);
  if_descriptor->iInterface         = *(uint8_t *)(buf + 8);
}