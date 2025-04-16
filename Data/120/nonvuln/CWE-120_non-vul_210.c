void
dao_ack_output(rpl_instance_t *instance, uip_ipaddr_t *dest, uint8_t sequence,
               uint8_t status)
{
#if RPL_WITH_DAO_ACK
  unsigned char *buffer;

  LOG_INFO("Sending a DAO %s with sequence number %d to ", status < 128 ? "ACK" : "NACK", sequence);
  LOG_INFO_6ADDR(dest);
  LOG_INFO_(" with status %d\n", status);

  buffer = UIP_ICMP_PAYLOAD;

  buffer[0] = instance->instance_id;
  buffer[1] = 0;
  buffer[2] = sequence;
  buffer[3] = status;

  uip_icmp6_send(dest, ICMP6_RPL, RPL_CODE_DAO_ACK, 4);
#endif /* RPL_WITH_DAO_ACK */
}