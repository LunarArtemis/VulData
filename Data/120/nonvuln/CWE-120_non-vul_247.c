static void USBH_ParseStringDesc(uint8_t *psrc, uint8_t *pdest, uint16_t length)
{
  uint16_t strlength;
  uint16_t idx;

  /* The UNICODE string descriptor is not NULL-terminated. The string length is
  computed by subtracting two from the value of the first byte of the descriptor.
  */

  /* Check which is lower size, the Size of string or the length of bytes read
  from the device */

  if (psrc[1] == USB_DESC_TYPE_STRING)
  {
    /* Make sure the Descriptor is String Type */

    /* psrc[0] contains Size of Descriptor, subtract 2 to get the length of string */
    strlength = ((((uint16_t)psrc[0] - 2U) <= length) ? ((uint16_t)psrc[0] - 2U) : length);

    /* Adjust the offset ignoring the String Len and Descriptor type */
    psrc += 2U;

    for (idx = 0U; idx < strlength; idx += 2U)
    {
      /* Copy Only the string and ignore the UNICODE ID, hence add the src */
      *pdest =  psrc[idx];
      pdest++;
    }
    *pdest = 0U; /* mark end of string */
  }
}