static void
dao_output_target_seq(rpl_parent_t *parent, uip_ipaddr_t *prefix,
                      uint8_t lifetime, uint8_t seq_no)
{
  rpl_dag_t *dag;
  rpl_instance_t *instance;
  unsigned char *buffer;
  uint8_t prefixlen;
  int pos;
  uip_ipaddr_t *parent_ipaddr = NULL;
  uip_ipaddr_t *dest_ipaddr = NULL;

  /* Destination Advertisement Object */

  /* If we are in feather mode, we should not send any DAOs */
  if(rpl_get_mode() == RPL_MODE_FEATHER) {
    return;
  }

  if(parent == NULL) {
    LOG_ERR("dao_output_target error parent NULL\n");
    return;
  }

  parent_ipaddr = rpl_parent_get_ipaddr(parent);
  if(parent_ipaddr == NULL) {
    LOG_ERR("dao_output_target error parent IP address NULL\n");
    return;
  }

  dag = parent->dag;
  if(dag == NULL) {
    LOG_ERR("dao_output_target error dag NULL\n");
    return;
  }

  instance = dag->instance;

  if(instance == NULL) {
    LOG_ERR("dao_output_target error instance NULL\n");
    return;
  }
  if(prefix == NULL) {
    LOG_ERR("dao_output_target error prefix NULL\n");
    return;
  }
#ifdef RPL_DEBUG_DAO_OUTPUT
  RPL_DEBUG_DAO_OUTPUT(parent);
#endif

  buffer = UIP_ICMP_PAYLOAD;
  pos = 0;

  buffer[pos++] = instance->instance_id;
  buffer[pos] = 0;
#if RPL_DAO_SPECIFY_DAG
  buffer[pos] |= RPL_DAO_D_FLAG;
#endif /* RPL_DAO_SPECIFY_DAG */
#if RPL_WITH_DAO_ACK
  if(lifetime != RPL_ZERO_LIFETIME) {
    buffer[pos] |= RPL_DAO_K_FLAG;
  }
#endif /* RPL_WITH_DAO_ACK */
  ++pos;
  buffer[pos++] = 0; /* reserved */
  buffer[pos++] = seq_no;
#if RPL_DAO_SPECIFY_DAG
  memcpy(buffer + pos, &dag->dag_id, sizeof(dag->dag_id));
  pos+=sizeof(dag->dag_id);
#endif /* RPL_DAO_SPECIFY_DAG */

  /* create target subopt */
  prefixlen = sizeof(*prefix) * CHAR_BIT;
  buffer[pos++] = RPL_OPTION_TARGET;
  buffer[pos++] = 2 + ((prefixlen + 7) / CHAR_BIT);
  buffer[pos++] = 0; /* reserved */
  buffer[pos++] = prefixlen;
  memcpy(buffer + pos, prefix, (prefixlen + 7) / CHAR_BIT);
  pos += ((prefixlen + 7) / CHAR_BIT);

  /* Create a transit information sub-option. */
  buffer[pos++] = RPL_OPTION_TRANSIT;
  buffer[pos++] = (instance->mop != RPL_MOP_NON_STORING) ? 4 : 20;
  buffer[pos++] = 0; /* flags - ignored */
  buffer[pos++] = 0; /* path control - ignored */
  buffer[pos++] = 0; /* path seq - ignored */
  buffer[pos++] = lifetime;

  if(instance->mop != RPL_MOP_NON_STORING) {
    /* Send DAO to parent */
    dest_ipaddr = parent_ipaddr;
  } else {
    /* Include parent global IP address */
    memcpy(buffer + pos, &parent->dag->dag_id, 8); /* Prefix */
    pos += 8;
    memcpy(buffer + pos, ((const unsigned char *)parent_ipaddr) + 8, 8); /* Interface identifier */
    pos += 8;
    /* Send DAO to root */
    dest_ipaddr = &parent->dag->dag_id;
  }

  LOG_INFO("Sending a %sDAO with sequence number %u, lifetime %u, prefix ",
         lifetime == RPL_ZERO_LIFETIME ? "No-Path " : "", seq_no, lifetime);

  LOG_INFO_6ADDR(prefix);
  LOG_INFO_(" to ");
  LOG_INFO_6ADDR(dest_ipaddr);
  LOG_INFO_(" , parent ");
  LOG_INFO_6ADDR(parent_ipaddr);
  LOG_INFO_("\n");

  if(dest_ipaddr != NULL) {
    uip_icmp6_send(dest_ipaddr, ICMP6_RPL, RPL_CODE_DAO, pos);
  }
}