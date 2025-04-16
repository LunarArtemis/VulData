static uint64_t stellaris_enet_read(void *opaque, hwaddr offset,
                                    unsigned size)
{
    stellaris_enet_state *s = (stellaris_enet_state *)opaque;
    uint32_t val;

    switch (offset) {
    case 0x00: /* RIS */
        DPRINTF("IRQ status %02x\n", s->ris);
        return s->ris;
    case 0x04: /* IM */
        return s->im;
    case 0x08: /* RCTL */
        return s->rctl;
    case 0x0c: /* TCTL */
        return s->tctl;
    case 0x10: /* DATA */
    {
        uint8_t *rx_fifo;

        if (s->np == 0) {
            BADF("RX underflow\n");
            return 0;
        }

        rx_fifo = s->rx[s->next_packet].data + s->rx_fifo_offset;

        val = rx_fifo[0] | (rx_fifo[1] << 8) | (rx_fifo[2] << 16)
              | (rx_fifo[3] << 24);
        s->rx_fifo_offset += 4;
        if (s->rx_fifo_offset >= s->rx[s->next_packet].len) {
            s->rx_fifo_offset = 0;
            s->next_packet++;
            if (s->next_packet >= 31)
                s->next_packet = 0;
            s->np--;
            DPRINTF("RX done np=%d\n", s->np);
            if (!s->np && stellaris_enet_can_receive(s)) {
                qemu_flush_queued_packets(qemu_get_queue(s->nic));
            }
        }
        return val;
    }
    case 0x14: /* IA0 */
        return s->conf.macaddr.a[0] | (s->conf.macaddr.a[1] << 8)
            | (s->conf.macaddr.a[2] << 16)
            | ((uint32_t)s->conf.macaddr.a[3] << 24);
    case 0x18: /* IA1 */
        return s->conf.macaddr.a[4] | (s->conf.macaddr.a[5] << 8);
    case 0x1c: /* THR */
        return s->thr;
    case 0x20: /* MCTL */
        return s->mctl;
    case 0x24: /* MDV */
        return s->mdv;
    case 0x28: /* MADD */
        return 0;
    case 0x2c: /* MTXD */
        return s->mtxd;
    case 0x30: /* MRXD */
        return s->mrxd;
    case 0x34: /* NP */
        return s->np;
    case 0x38: /* TR */
        return 0;
    case 0x3c: /* Undocuented: Timestamp? */
        return 0;
    default:
        hw_error("stellaris_enet_read: Bad offset %x\n", (int)offset);
        return 0;
    }
}