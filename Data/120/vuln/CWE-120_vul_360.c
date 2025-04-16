int startControlStream(void) {
    int err;

    PltCreateMutex(&enetMutex);

    if (ServerMajorVersion >= 5) {
        ENetAddress address;
        ENetEvent event;
        
        enet_address_set_address(&address, (struct sockaddr *)&RemoteAddr, RemoteAddrLen);
        enet_address_set_port(&address, 47999);

        // Create a client that can use 1 outgoing connection and 1 channel
        client = enet_host_create(address.address.ss_family, NULL, 1, 1, 0, 0);
        if (client == NULL) {
            return -1;
        }

        // Connect to the host
        peer = enet_host_connect(client, &address, 1, 0);
        if (peer == NULL) {
            enet_host_destroy(client);
            client = NULL;
            return -1;
        }

        // Wait for the connect to complete
        if (serviceEnetHost(client, &event, CONTROL_STREAM_TIMEOUT_SEC * 1000) <= 0 ||
            event.type != ENET_EVENT_TYPE_CONNECT) {
            Limelog("RTSP: Failed to connect to UDP port 47999\n");
            enet_peer_reset(peer);
            peer = NULL;
            enet_host_destroy(client);
            client = NULL;
            return -1;
        }

        // Ensure the connect verify ACK is sent immediately
        enet_host_flush(client);
        
        // Set the max peer timeout to 10 seconds
        enet_peer_timeout(peer, ENET_PEER_TIMEOUT_LIMIT, ENET_PEER_TIMEOUT_MINIMUM, 10000);
    }
    else {
        ctlSock = connectTcpSocket(&RemoteAddr, RemoteAddrLen,
            47995, CONTROL_STREAM_TIMEOUT_SEC);
        if (ctlSock == INVALID_SOCKET) {
            return LastSocketFail();
        }

        enableNoDelay(ctlSock);
    }

    // Send START A
    if (!sendMessageAndDiscardReply(packetTypes[IDX_START_A],
        payloadLengths[IDX_START_A],
        preconstructedPayloads[IDX_START_A])) {
        Limelog("Start A failed: %d\n", (int)LastSocketError());
        return LastSocketFail();
    }

    // Send START B
    if (!sendMessageAndDiscardReply(packetTypes[IDX_START_B],
        payloadLengths[IDX_START_B],
        preconstructedPayloads[IDX_START_B])) {
        Limelog("Start B failed: %d\n", (int)LastSocketError());
        return LastSocketFail();
    }

    err = PltCreateThread(lossStatsThreadFunc, NULL, &lossStatsThread);
    if (err != 0) {
        return err;
    }

    err = PltCreateThread(invalidateRefFramesFunc, NULL, &invalidateRefFramesThread);
    if (err != 0) {
        return err;
    }

    return 0;
}