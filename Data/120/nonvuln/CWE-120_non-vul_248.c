static void  USBH_ParseDevDesc(USBH_DevDescTypeDef *dev_desc, uint8_t *buf,
                               uint16_t length)
{
  dev_desc->bLength            = *(uint8_t *)(buf +  0);
  dev_desc->bDescriptorType    = *(uint8_t *)(buf +  1);
  dev_desc->bcdUSB             = LE16(buf +  2);
  dev_desc->bDeviceClass       = *(uint8_t *)(buf +  4);
  dev_desc->bDeviceSubClass    = *(uint8_t *)(buf +  5);
  dev_desc->bDeviceProtocol    = *(uint8_t *)(buf +  6);
  dev_desc->bMaxPacketSize     = *(uint8_t *)(buf +  7);

  /* Make sure that the max packet size is either 8, 16, 32, 64 or force it to 64 */
  switch (dev_desc->bMaxPacketSize)
  {
    case 8:
    case 16:
    case 32:
    case 64:
      dev_desc->bMaxPacketSize = dev_desc->bMaxPacketSize;
      break;

    default:
      /*set the size to 64 in case the device has answered with incorrect size */
      dev_desc->bMaxPacketSize = 64U;
      break;
  }

  if (length > 8U)
  {
    /* For 1st time after device connection, Host may issue only 8 bytes for
    Device Descriptor Length  */
    dev_desc->idVendor           = LE16(buf +  8);
    dev_desc->idProduct          = LE16(buf + 10);
    dev_desc->bcdDevice          = LE16(buf + 12);
    dev_desc->iManufacturer      = *(uint8_t *)(buf + 14);
    dev_desc->iProduct           = *(uint8_t *)(buf + 15);
    dev_desc->iSerialNumber      = *(uint8_t *)(buf + 16);
    dev_desc->bNumConfigurations = *(uint8_t *)(buf + 17);
  }
}