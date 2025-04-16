int startInputStream(void) {
    int err;

    // After Gen 5, we send input on the control stream
    if (ServerMajorVersion < 5) {
        inputSock = connectTcpSocket(&RemoteAddr, RemoteAddrLen,
            35043, INPUT_STREAM_TIMEOUT_SEC);
        if (inputSock == INVALID_SOCKET) {
            return LastSocketFail();
        }

        enableNoDelay(inputSock);
    }

    err = PltCreateThread(inputSendThreadProc, NULL, &inputSendThread);
    if (err != 0) {
        return err;
    }

    return err;
}