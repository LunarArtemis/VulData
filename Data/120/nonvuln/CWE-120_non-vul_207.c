uip_ds6_nbr_t *
rpl_icmp6_update_nbr_table(uip_ipaddr_t *from, nbr_table_reason_t reason, void *data)
{
  uip_ds6_nbr_t *nbr;

  if((nbr = uip_ds6_nbr_lookup(from)) == NULL) {
    if((nbr = uip_ds6_nbr_add(from, (uip_lladdr_t *)
                              packetbuf_addr(PACKETBUF_ADDR_SENDER),
                              0, NBR_REACHABLE, reason, data)) != NULL) {
      LOG_INFO("Neighbor added to neighbor cache ");
      LOG_INFO_6ADDR(from);
      LOG_INFO_(", ");
      LOG_INFO_LLADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
      LOG_INFO_("\n");
    }
  }

  return nbr;
}