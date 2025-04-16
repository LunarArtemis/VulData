static int transactRtspMessage(PRTSP_MESSAGE request, PRTSP_MESSAGE response, int expectingPayload, int* error) {
    // Gen 5+ does RTSP over ENet not TCP
    if (ServerMajorVersion >= 5) {
        return transactRtspMessageEnet(request, response, expectingPayload, error);
    }
    else {
        return transactRtspMessageTcp(request, response, expectingPayload, error);
    }
}