USBH_DescHeader_t  *USBH_GetNextDesc(uint8_t   *pbuf, uint16_t  *ptr)
{
  USBH_DescHeader_t  *pnext;

  *ptr += ((USBH_DescHeader_t *)(void *)pbuf)->bLength;
  pnext = (USBH_DescHeader_t *)(void *)((uint8_t *)(void *)pbuf + \
                                        ((USBH_DescHeader_t *)(void *)pbuf)->bLength);

  return (pnext);
}