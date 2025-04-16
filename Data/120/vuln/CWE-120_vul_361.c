static int sendMessageAndDiscardReply(short ptype, short paylen, const void* payload) {
    // Discard the response
    if (ServerMajorVersion >= 5) {
        ENetEvent event;

        PltLockMutex(&enetMutex);

        if (!sendMessageEnet(ptype, paylen, payload)) {
            PltUnlockMutex(&enetMutex);
            return 0;
        }

        if (serviceEnetHost(client, &event, CONTROL_STREAM_TIMEOUT_SEC * 1000) <= 0 ||
            event.type != ENET_EVENT_TYPE_RECEIVE) {
            PltUnlockMutex(&enetMutex);
            return 0;
        }

        enet_packet_destroy(event.packet);

        PltUnlockMutex(&enetMutex);
    }
    else {
        PNVCTL_TCP_PACKET_HEADER reply;

        if (!sendMessageTcp(ptype, paylen, payload)) {
            return 0;
        }

        reply = readNvctlPacketTcp();
        if (reply == NULL) {
            return 0;
        }

        free(reply);
    }

    return 1;
}