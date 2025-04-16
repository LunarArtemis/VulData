static void
e1000_send_packet(E1000State *s, const uint8_t *buf, int size)
{
    if (s->phy_reg[PHY_CTRL] & MII_CR_LOOPBACK) {
        s->nic->nc.info->receive(&s->nic->nc, buf, size);
    } else {
        qemu_send_packet(&s->nic->nc, buf, size);
    }
}