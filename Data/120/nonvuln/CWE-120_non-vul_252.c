static USBH_StatusTypeDef USBH_ParseCfgDesc(USBH_HandleTypeDef *phost, uint8_t *buf, uint16_t length)
{
  USBH_CfgDescTypeDef *cfg_desc = &phost->device.CfgDesc;
  USBH_StatusTypeDef           status = USBH_OK;
  USBH_InterfaceDescTypeDef    *pif ;
  USBH_EpDescTypeDef           *pep;
  USBH_DescHeader_t            *pdesc = (USBH_DescHeader_t *)(void *)buf;
  uint16_t                     ptr;
  uint8_t                      if_ix = 0U;
  uint8_t                      ep_ix = 0U;

  pdesc   = (USBH_DescHeader_t *)(void *)buf;

  /* Parse configuration descriptor */
  cfg_desc->bLength             = *(uint8_t *)(buf + 0);
  cfg_desc->bDescriptorType     = *(uint8_t *)(buf + 1);
  cfg_desc->wTotalLength        = MIN(((uint16_t) LE16(buf + 2)), ((uint16_t)USBH_MAX_SIZE_CONFIGURATION));
  cfg_desc->bNumInterfaces      = *(uint8_t *)(buf + 4);
  cfg_desc->bConfigurationValue = *(uint8_t *)(buf + 5);
  cfg_desc->iConfiguration      = *(uint8_t *)(buf + 6);
  cfg_desc->bmAttributes        = *(uint8_t *)(buf + 7);
  cfg_desc->bMaxPower           = *(uint8_t *)(buf + 8);

  /* Make sure that the Confguration descriptor's bLength is equal to USB_CONFIGURATION_DESC_SIZE */
  if (cfg_desc->bLength  != USB_CONFIGURATION_DESC_SIZE)
  {
    cfg_desc->bLength = USB_CONFIGURATION_DESC_SIZE;
  }

  if (length > USB_CONFIGURATION_DESC_SIZE)
  {
    ptr = USB_LEN_CFG_DESC;
    pif = (USBH_InterfaceDescTypeDef *)NULL;

    while ((if_ix < USBH_MAX_NUM_INTERFACES) && (ptr < cfg_desc->wTotalLength))
    {
      pdesc = USBH_GetNextDesc((uint8_t *)(void *)pdesc, &ptr);
      if (pdesc->bDescriptorType == USB_DESC_TYPE_INTERFACE)
      {
        /* Make sure that the interface descriptor's bLength is equal to USB_INTERFACE_DESC_SIZE */
        if (pdesc->bLength != USB_INTERFACE_DESC_SIZE)
        {
          pdesc->bLength = USB_INTERFACE_DESC_SIZE;
        }

        pif = &cfg_desc->Itf_Desc[if_ix];
        USBH_ParseInterfaceDesc(pif, (uint8_t *)(void *)pdesc);

        ep_ix = 0U;
        pep = (USBH_EpDescTypeDef *)NULL;

        while ((ep_ix < pif->bNumEndpoints) && (ptr < cfg_desc->wTotalLength))
        {
          pdesc = USBH_GetNextDesc((uint8_t *)(void *)pdesc, &ptr);

          if (pdesc->bDescriptorType == USB_DESC_TYPE_ENDPOINT)
          {
            /* Check if the endpoint is appartening to an audio streaming interface */
            if ((pif->bInterfaceClass == 0x01U) && (pif->bInterfaceSubClass == 0x02U))
            {
              /* Check if it is supporting the USB AUDIO 01 class specification */
              if ((pif->bInterfaceProtocol == 0x00U) && (pdesc->bLength != 0x09U))
              {
                pdesc->bLength = 0x09U;
              }
            }
            /* Make sure that the endpoint descriptor's bLength is equal to
               USB_ENDPOINT_DESC_SIZE for all other endpoints types */
            else if (pdesc->bLength != USB_ENDPOINT_DESC_SIZE)
            {
              pdesc->bLength = USB_ENDPOINT_DESC_SIZE;
            }
            else
            {
              /* ... */
            }

            pep = &cfg_desc->Itf_Desc[if_ix].Ep_Desc[ep_ix];

            status = USBH_ParseEPDesc(phost, pep, (uint8_t *)(void *)pdesc);

            ep_ix++;
          }
        }

        /* Check if the required endpoint(s) data are parsed */
        if (ep_ix < pif->bNumEndpoints)
        {
          return USBH_NOT_SUPPORTED;
        }

        if_ix++;
      }
    }

    /* Check if the required interface(s) data are parsed */
    if (if_ix < MIN(cfg_desc->bNumInterfaces, (uint8_t)USBH_MAX_NUM_INTERFACES))
    {
      return USBH_NOT_SUPPORTED;
    }
  }

  return status;
}