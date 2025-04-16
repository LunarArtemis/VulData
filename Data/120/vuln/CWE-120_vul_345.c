int LiSendMouseButtonEvent(char action, int button) {
    PPACKET_HOLDER holder;
    int err;

    if (!initialized) {
        return -2;
    }

    holder = malloc(sizeof(*holder));
    if (holder == NULL) {
        return -1;
    }

    holder->packetLength = sizeof(NV_MOUSE_BUTTON_PACKET);
    holder->packet.mouseButton.header.packetType = htonl(PACKET_TYPE_MOUSE_BUTTON);
    holder->packet.mouseButton.action = action;
    if (ServerMajorVersion >= 5) {
        holder->packet.mouseButton.action++;
    }
    holder->packet.mouseButton.button = htonl(button);

    err = LbqOfferQueueItem(&packetQueue, holder, &holder->entry);
    if (err != LBQ_SUCCESS) {
        free(holder);
    }

    return err;
}