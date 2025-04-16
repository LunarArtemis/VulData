static int sendMessageTcp(short ptype, short paylen, const void* payload) {
    PNVCTL_TCP_PACKET_HEADER packet;
    SOCK_RET err;

    LC_ASSERT(ServerMajorVersion < 5);

    packet = malloc(sizeof(*packet) + paylen);
    if (packet == NULL) {
        return 0;
    }

    packet->type = ptype;
    packet->payloadLength = paylen;
    memcpy(&packet[1], payload, paylen);

    err = send(ctlSock, (char*) packet, sizeof(*packet) + paylen, 0);
    free(packet);

    if (err != sizeof(*packet) + paylen) {
        return 0;
    }

    return 1;
}