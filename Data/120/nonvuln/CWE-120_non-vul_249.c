USBH_StatusTypeDef USBH_CtlReq(USBH_HandleTypeDef *phost, uint8_t *buff,
                               uint16_t length)
{
  USBH_StatusTypeDef status;
  status = USBH_BUSY;

  switch (phost->RequestState)
  {
    case CMD_SEND:
      /* Start a SETUP transfer */
      phost->Control.buff = buff;
      phost->Control.length = length;
      phost->Control.state = CTRL_SETUP;
      phost->RequestState = CMD_WAIT;
      status = USBH_BUSY;

#if (USBH_USE_OS == 1U)
      phost->os_msg = (uint32_t)USBH_CONTROL_EVENT;
#if (osCMSIS < 0x20000U)
      (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
      (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
      break;

    case CMD_WAIT:
      status = USBH_HandleControl(phost);
      if ((status == USBH_OK) || (status == USBH_NOT_SUPPORTED))
      {
        /* Transaction completed, move control state to idle */
        phost->RequestState = CMD_SEND;
        phost->Control.state = CTRL_IDLE;
      }
      else if (status == USBH_FAIL)
      {
        /* Failure Mode */
        phost->RequestState = CMD_SEND;
      }
      else
      {
        /* .. */
      }
#if (USBH_USE_OS == 1U)
      phost->os_msg = (uint32_t)USBH_CONTROL_EVENT;
#if (osCMSIS < 0x20000U)
      (void)osMessagePut(phost->os_event, phost->os_msg, 0U);
#else
      (void)osMessageQueuePut(phost->os_event, &phost->os_msg, 0U, 0U);
#endif
#endif
      break;

    default:
      break;
  }
  return status;
}