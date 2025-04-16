int LiSendScrollEvent(signed char scrollClicks) {
    PPACKET_HOLDER holder;
    int err;

    if (!initialized) {
        return -2;
    }

    holder = malloc(sizeof(*holder));
    if (holder == NULL) {
        return -1;
    }

    holder->packetLength = sizeof(NV_SCROLL_PACKET);
    holder->packet.scroll.header.packetType = htonl(PACKET_TYPE_SCROLL);
    holder->packet.scroll.magicA = MAGIC_A;
    // On Gen 5 servers, the header code is incremented by one
    if (ServerMajorVersion >= 5) {
        holder->packet.scroll.magicA++;
    }
    holder->packet.scroll.zero1 = 0;
    holder->packet.scroll.zero2 = 0;
    holder->packet.scroll.scrollAmt1 = htons(scrollClicks * 120);
    holder->packet.scroll.scrollAmt2 = holder->packet.scroll.scrollAmt1;
    holder->packet.scroll.zero3 = 0;

    err = LbqOfferQueueItem(&packetQueue, holder, &holder->entry);
    if (err != LBQ_SUCCESS) {
        free(holder);
    }

    return err;
}