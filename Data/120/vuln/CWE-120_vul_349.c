int LiSendMouseMoveEvent(short deltaX, short deltaY) {
    PPACKET_HOLDER holder;
    int err;

    if (!initialized) {
        return -2;
    }

    holder = malloc(sizeof(*holder));
    if (holder == NULL) {
        return -1;
    }

    holder->packetLength = sizeof(NV_MOUSE_MOVE_PACKET);
    holder->packet.mouseMove.header.packetType = htonl(PACKET_TYPE_MOUSE_MOVE);
    holder->packet.mouseMove.magic = MOUSE_MOVE_MAGIC;
    // On Gen 5 servers, the header code is incremented by one
    if (ServerMajorVersion >= 5) {
        holder->packet.mouseMove.magic++;
    }
    holder->packet.mouseMove.deltaX = htons(deltaX);
    holder->packet.mouseMove.deltaY = htons(deltaY);

    err = LbqOfferQueueItem(&packetQueue, holder, &holder->entry);
    if (err != LBQ_SUCCESS) {
        free(holder);
    }

    return err;
}