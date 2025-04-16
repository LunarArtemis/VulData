static inline bool gnrc_rpl_validation_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack,
                                               uint16_t len,
                                               ipv6_addr_t *dst)
{
    uint16_t expected_len = sizeof(*dao_ack) + sizeof(icmpv6_hdr_t);

    if (ipv6_addr_is_multicast(dst)) {
        DEBUG("RPL: received DAO-ACK on multicast address\n");
        return false;
    }

    if ((dao_ack->d_reserved & GNRC_RPL_DAO_ACK_D_BIT)) {
        expected_len += sizeof(ipv6_addr_t);
    }

    if (expected_len == len) {
        return true;
    }

    DEBUG("RPL: wrong DAO-ACK len: %d, expected: %d\n", len, expected_len);

    return false;
}