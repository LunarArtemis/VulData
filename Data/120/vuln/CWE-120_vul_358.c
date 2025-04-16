int startVideoStream(void* rendererContext, int drFlags) {
    int err;

    // This must be called before the decoder thread starts submitting
    // decode units
    LC_ASSERT(NegotiatedVideoFormat != 0);
    VideoCallbacks.setup(NegotiatedVideoFormat, StreamConfig.width,
        StreamConfig.height, StreamConfig.fps, rendererContext, drFlags);

    rtpSocket = bindUdpSocket(RemoteAddr.ss_family, RTP_RECV_BUFFER);
    if (rtpSocket == INVALID_SOCKET) {
        return LastSocketError();
    }

    err = PltCreateThread(ReceiveThreadProc, NULL, &receiveThread);
    if (err != 0) {
        return err;
    }

    if ((VideoCallbacks.capabilities & CAPABILITY_DIRECT_SUBMIT) == 0) {
        err = PltCreateThread(DecoderThreadProc, NULL, &decoderThread);
        if (err != 0) {
            return err;
        }
    }

    if (ServerMajorVersion == 3) {
        // Connect this socket to open port 47998 for our ping thread
        firstFrameSocket = connectTcpSocket(&RemoteAddr, RemoteAddrLen,
                                            FIRST_FRAME_PORT, FIRST_FRAME_TIMEOUT_SEC);
        if (firstFrameSocket == INVALID_SOCKET) {
            return LastSocketError();
        }
    }

    // Start pinging before reading the first frame so GFE knows where
    // to send UDP data
    err = PltCreateThread(UdpPingThreadProc, NULL, &udpPingThread);
    if (err != 0) {
        return err;
    }

    if (ServerMajorVersion == 3) {
        // Read the first frame to start the flow of video
        err = readFirstFrame();
        if (err != 0) {
            return err;
        }
    }

    return 0;
}