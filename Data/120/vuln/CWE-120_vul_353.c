static int setupStream(PRTSP_MESSAGE response, char* target, int* error) {
    RTSP_MESSAGE request;
    int ret;
    char* transportValue;

    *error = -1;

    ret = initializeRtspRequest(&request, "SETUP", target);
    if (ret != 0) {
        if (hasSessionId) {
            if (!addOption(&request, "Session", sessionIdString)) {
                ret = 0;
                goto FreeMessage;
            }
        }

        if (ServerMajorVersion >= 6) {
            // It looks like GFE doesn't care what we say our port is but
            // we need to give it some port to successfully complete the
            // handshake process.
            transportValue = "unicast;X-GS-ClientPort=50000-50001";
        }
        else {
            transportValue = " ";
        }
        
        if (addOption(&request, "Transport", transportValue) &&
            addOption(&request, "If-Modified-Since",
                "Thu, 01 Jan 1970 00:00:00 GMT")) {
            ret = transactRtspMessage(&request, response, 0, error);
        }
        else {
            ret = 0;
        }

    FreeMessage:
        freeMessage(&request);
    }

    return ret;
}