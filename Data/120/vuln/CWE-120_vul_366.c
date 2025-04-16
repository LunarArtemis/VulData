static int sendMessageEnet(short ptype, short paylen, const void* payload) {
    PNVCTL_ENET_PACKET_HEADER packet;
    ENetPacket* enetPacket;
    ENetEvent event;
    int err;

    LC_ASSERT(ServerMajorVersion >= 5);
    
    // We may be trying to disconnect, so our peer could be gone.
    // This check is safe because we're guaranteed to be holding enetMutex.
    if (peer == NULL) {
        return 0;
    }

    packet = malloc(sizeof(*packet) + paylen);
    if (packet == NULL) {
        return 0;
    }

    packet->type = ptype;
    memcpy(&packet[1], payload, paylen);

    // Gen 5+ servers do control protocol over ENet instead of TCP
    while ((err = serviceEnetHost(client, &event, 0)) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event.packet);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            Limelog("Control stream received disconnect event\n");
            free(packet);
            return 0;
        }
    }
    
    if (err < 0) {
        Limelog("Control stream connection failed\n");
        return 0;
    }

    enetPacket = enet_packet_create(packet, sizeof(*packet) + paylen, ENET_PACKET_FLAG_RELIABLE);
    if (packet == NULL) {
        free(packet);
        return 0;
    }

    if (enet_peer_send(peer, 0, enetPacket) < 0) {
        Limelog("Failed to send ENet control packet\n");
        enet_packet_destroy(enetPacket);
        free(packet);
        return 0;
    }
    
    enet_host_flush(client);

    free(packet);

    return 1;
}