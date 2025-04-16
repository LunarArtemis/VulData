static int stellaris_enet_post_load(void *opaque, int version_id)
{
    stellaris_enet_state *s = opaque;
    int i;

    /* Sanitize inbound state. Note that next_packet is an index but
     * np is a size; hence their valid upper bounds differ.
     */
    if (s->next_packet >= ARRAY_SIZE(s->rx)) {
        return -1;
    }

    if (s->np > ARRAY_SIZE(s->rx)) {
        return -1;
    }

    for (i = 0; i < ARRAY_SIZE(s->rx); i++) {
        if (s->rx[i].len > ARRAY_SIZE(s->rx[i].data)) {
            return -1;
        }
    }

    if (s->rx_fifo_offset > ARRAY_SIZE(s->rx[0].data) - 4) {
        return -1;
    }

    if (s->tx_fifo_len > ARRAY_SIZE(s->tx_fifo)) {
        return -1;
    }

    return 0;
}