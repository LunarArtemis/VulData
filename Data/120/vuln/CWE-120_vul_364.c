static void requestIdrFrame(void) {
    long long payload[3];

    if (ServerMajorVersion >= 5) {
        // Form the payload
        if (lastSeenFrame < 0x20) {
            payload[0] = 0;
            payload[1] = 0x20;
        }
        else {
            payload[0] = lastSeenFrame - 0x20;
            payload[1] = lastSeenFrame;
        }

        payload[2] = 0;

        // Send the reference frame invalidation request and read the response
        if (!sendMessageAndDiscardReply(packetTypes[IDX_INVALIDATE_REF_FRAMES],
            payloadLengths[IDX_INVALIDATE_REF_FRAMES], payload)) {
            Limelog("Request IDR Frame: Transaction failed: %d\n", (int)LastSocketError());
            ListenerCallbacks.connectionTerminated(LastSocketError());
            return;
        }
    }
    else {
        // Send IDR frame request and read the response
        if (!sendMessageAndDiscardReply(packetTypes[IDX_REQUEST_IDR_FRAME],
            payloadLengths[IDX_REQUEST_IDR_FRAME], preconstructedPayloads[IDX_REQUEST_IDR_FRAME])) {
            Limelog("Request IDR Frame: Transaction failed: %d\n", (int)LastSocketError());
            ListenerCallbacks.connectionTerminated(LastSocketError());
            return;
        }
    }

    Limelog("IDR frame request sent\n");
}