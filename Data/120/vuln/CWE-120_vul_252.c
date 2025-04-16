int
rpl_ext_header_srh_update(void)
{
#if RPL_WITH_NON_STORING
  struct uip_routing_hdr *rh_header;
  struct uip_rpl_srh_hdr *srh_header;

  /* Look for routing ext header */
  rh_header = (struct uip_routing_hdr *)uipbuf_search_header(uip_buf, uip_len, UIP_PROTO_ROUTING);

  if(rh_header != NULL && rh_header->routing_type == RPL_RH_TYPE_SRH) {
    /* SRH found, now look for next hop */
    uint8_t cmpri, cmpre;
    uint8_t ext_len;
    uint8_t padding;
    uint8_t path_len;
    uint8_t segments_left;
    uip_ipaddr_t current_dest_addr;

    srh_header = (struct uip_rpl_srh_hdr *)(((uint8_t *)rh_header) + RPL_RH_LEN);
    segments_left = rh_header->seg_left;
    ext_len = rh_header->len * 8 + 8;
    cmpri = srh_header->cmpr >> 4;
    cmpre = srh_header->cmpr & 0x0f;
    padding = srh_header->pad >> 4;
    path_len = ((ext_len - padding - RPL_RH_LEN - RPL_SRH_LEN - (16 - cmpre)) / (16 - cmpri)) + 1;
    (void)path_len;

    LOG_DBG("read SRH, path len %u, segments left %u, Cmpri %u, Cmpre %u, ext len %u (padding %u)\n",
        path_len, segments_left, cmpri, cmpre, ext_len, padding);

    if(segments_left == 0) {
      /* We are the final destination, do nothing */
    } else {
      uint8_t i = path_len - segments_left; /* The index of the next address to be visited */
      uint8_t *addr_ptr = ((uint8_t *)rh_header) + RPL_RH_LEN + RPL_SRH_LEN + (i * (16 - cmpri));
      uint8_t cmpr = segments_left == 1 ? cmpre : cmpri;

      /* As per RFC6554: swap the IPv6 destination address and address[i] */

      /* First, copy the current IPv6 destination address */
      uip_ipaddr_copy(&current_dest_addr, &UIP_IP_BUF->destipaddr);
      /* Second, update the IPv6 destination address with addresses[i] */
      memcpy(((uint8_t *)&UIP_IP_BUF->destipaddr) + cmpr, addr_ptr, 16 - cmpr);
      /* Third, write current_dest_addr to addresses[i] */
      memcpy(addr_ptr, ((uint8_t *)&current_dest_addr) + cmpr, 16 - cmpr);

      /* Update segments left field */
      rh_header->seg_left--;

      LOG_INFO("SRH next hop ");
      LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
      LOG_INFO_("\n");
    }
    return 1;
  }

  return 0;
#else /* RPL_WITH_NON_STORING */
  return 0; /* SRH not found */
#endif /* RPL_WITH_NON_STORING */
}