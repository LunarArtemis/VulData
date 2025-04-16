int sendInputPacketOnControlStream(unsigned char* data, int length) {
    LC_ASSERT(ServerMajorVersion >= 5);

    // Send the input data (no reply expected)
    if (sendMessageAndForget(packetTypes[IDX_INPUT_DATA], length, data) == 0) {
        return -1;
    }

    return 0;
}