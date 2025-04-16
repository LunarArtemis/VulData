static void
handle_dao_retransmission(void *ptr)
{
  rpl_parent_t *parent;
  uip_ipaddr_t prefix;
  rpl_instance_t *instance;

  parent = ptr;
  if(parent == NULL || parent->dag == NULL || parent->dag->instance == NULL) {
    return;
  }
  instance = parent->dag->instance;

  if(instance->my_dao_transmissions >= RPL_DAO_MAX_RETRANSMISSIONS) {
    /* No more retransmissions - give up. */
    if(instance->lifetime_unit == 0xffff && instance->default_lifetime == 0xff) {
      /*
       * ContikiRPL was previously using infinite lifetime for routes
       * and no DAO_ACK configured. This probably means that the root
       * and possibly other nodes might be running an old version that
       * does not support DAO ack. Assume that everything is ok for
       * now and let the normal repair mechanisms detect any problems.
       */
      return;
    }

    if(RPL_IS_STORING(instance) && instance->of->dao_ack_callback) {
      /* Inform the objective function about the timeout. */
      instance->of->dao_ack_callback(parent, RPL_DAO_ACK_TIMEOUT);
    }

    /* Perform local repair and hope to find another parent. */
    rpl_local_repair(instance);
    return;
  }

  LOG_INFO("will retransmit DAO - seq:%d trans:%d\n", instance->my_dao_seqno,
         instance->my_dao_transmissions);

  if(get_global_addr(&prefix) == 0) {
    return;
  }

  ctimer_set(&instance->dao_retransmit_timer,
             RPL_DAO_RETRANSMISSION_TIMEOUT / 2 +
             (random_rand() % (RPL_DAO_RETRANSMISSION_TIMEOUT / 2)),
             handle_dao_retransmission, parent);

  instance->my_dao_transmissions++;
  dao_output_target_seq(parent, &prefix,
                        instance->default_lifetime, instance->my_dao_seqno);
}