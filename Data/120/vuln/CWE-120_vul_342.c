int performRtspHandshake(void) {
    int ret;

    // HACK: In order to get GFE to respect our request for a lower audio bitrate, we must
    // fake our target address so it doesn't match any of the PC's local interfaces. It seems
    // that the only way to get it to give you "low quality" stereo audio nowadays is if it
    // thinks you are remote (target address != any local address).
    if (OriginalVideoBitrate >= HIGH_AUDIO_BITRATE_THRESHOLD &&
            (AudioCallbacks.capabilities & CAPABILITY_SLOW_OPUS_DECODER) == 0) {
        addrToUrlSafeString(&RemoteAddr, urlAddr);
    }
    else {
        strcpy(urlAddr, "0.0.0.0");
    }

    LC_ASSERT(RtspPortNumber != 0);

    // Initialize global state
    useEnet = (AppVersionQuad[0] >= 5) && (AppVersionQuad[0] <= 7) && (AppVersionQuad[2] < 404);
    sprintf(rtspTargetUrl, "rtsp%s://%s:%u", useEnet ? "ru" : "", urlAddr, RtspPortNumber);
    currentSeqNumber = 1;
    hasSessionId = false;
    controlStreamId = APP_VERSION_AT_LEAST(7, 1, 431) ? "streamid=control/13/0" : "streamid=control/1/0";
    AudioEncryptionEnabled = false;

    switch (AppVersionQuad[0]) {
        case 3:
            rtspClientVersion = 10;
            break;
        case 4:
            rtspClientVersion = 11;
            break;
        case 5:
            rtspClientVersion = 12;
            break;
        case 6:
            // Gen 6 has never been seen in the wild
            rtspClientVersion = 13;
            break;
        case 7:
        default:
            rtspClientVersion = 14;
            break;
    }
    
    // Setup ENet if required by this GFE version
    if (useEnet) {
        ENetAddress address;
        ENetEvent event;
        
        enet_address_set_address(&address, (struct sockaddr *)&RemoteAddr, RemoteAddrLen);
        enet_address_set_port(&address, RtspPortNumber);
        
        // Create a client that can use 1 outgoing connection and 1 channel
        client = enet_host_create(RemoteAddr.ss_family, NULL, 1, 1, 0, 0);
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
        if (serviceEnetHost(client, &event, RTSP_CONNECT_TIMEOUT_SEC * 1000) <= 0 ||
            event.type != ENET_EVENT_TYPE_CONNECT) {
            Limelog("RTSP: Failed to connect to UDP port %u\n", RtspPortNumber);
            enet_peer_reset(peer);
            peer = NULL;
            enet_host_destroy(client);
            client = NULL;
            return -1;
        }

        // Ensure the connect verify ACK is sent immediately
        enet_host_flush(client);
    }

    {
        RTSP_MESSAGE response;
        int error = -1;

        if (!requestOptions(&response, &error)) {
            Limelog("RTSP OPTIONS request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP OPTIONS request failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }

        freeMessage(&response);
    }

    {
        RTSP_MESSAGE response;
        int error = -1;

        if (!requestDescribe(&response, &error)) {
            Limelog("RTSP DESCRIBE request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP DESCRIBE request failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }
        
        // The RTSP DESCRIBE reply will contain a collection of SDP media attributes that
        // describe the various supported video stream formats and include the SPS, PPS,
        // and VPS (if applicable). We will use this information to determine whether the
        // server can support HEVC. For some reason, they still set the MIME type of the HEVC
        // format to H264, so we can't just look for the HEVC MIME type. What we'll do instead is
        // look for the base 64 encoded VPS NALU prefix that is unique to the HEVC bitstream.
        if (StreamConfig.supportsHevc && strstr(response.payload, "sprop-parameter-sets=AAAAAU")) {
            if (StreamConfig.enableHdr) {
                NegotiatedVideoFormat = VIDEO_FORMAT_H265_MAIN10;
            }
            else {
                NegotiatedVideoFormat = VIDEO_FORMAT_H265;

                // Apply bitrate adjustment for SDR HEVC if the client requested one
                if (StreamConfig.hevcBitratePercentageMultiplier != 0) {
                    StreamConfig.bitrate *= StreamConfig.hevcBitratePercentageMultiplier;
                    StreamConfig.bitrate /= 100;
                }
            }
        }
        else {
            NegotiatedVideoFormat = VIDEO_FORMAT_H264;

            // Dimensions over 4096 are only supported with HEVC on NVENC
            if (StreamConfig.width > 4096 || StreamConfig.height > 4096) {
                Limelog("WARNING: Host PC doesn't support HEVC. Streaming at resolutions above 4K using H.264 will likely fail!\n");
            }
        }

        // Look for the SDP attribute that indicates we're dealing with a server that supports RFI
        ReferenceFrameInvalidationSupported = strstr(response.payload, "x-nv-video[0].refPicInvalidation") != NULL;
        if (!ReferenceFrameInvalidationSupported) {
            Limelog("Reference frame invalidation is not supported by this host\n");
        }

        // Parse the Opus surround parameters out of the RTSP DESCRIBE response.
        ret = parseOpusConfigurations(&response);
        if (ret != 0) {
            goto Exit;
        }

        freeMessage(&response);
    }

    {
        RTSP_MESSAGE response;
        char* sessionId;
        int error = -1;

        if (!setupStream(&response,
                         AppVersionQuad[0] >= 5 ? "streamid=audio/0/0" : "streamid=audio",
                         &error)) {
            Limelog("RTSP SETUP streamid=audio request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP SETUP streamid=audio request failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }

        // Parse the audio port out of the RTSP SETUP response
        LC_ASSERT(AudioPortNumber == 0);
        if (!parseServerPortFromTransport(&response, &AudioPortNumber)) {
            // Use the well known port if parsing fails
            AudioPortNumber = 48000;

            Limelog("Audio port: %u (RTSP parsing failed)\n", AudioPortNumber);
        }
        else {
            Limelog("Audio port: %u\n", AudioPortNumber);
        }

        // Let the audio stream know the port number is now finalized.
        // NB: This is needed because audio stream init happens before RTSP,
        // which is not the case for the video stream.
        notifyAudioPortNegotiationComplete();

        sessionId = getOptionContent(response.options, "Session");

        if (sessionId == NULL) {
            Limelog("RTSP SETUP streamid=audio is missing session attribute\n");
            ret = -1;
            goto Exit;
        }

        // Given there is a non-null session id, get the
        // first token of the session until ";", which 
        // resolves any 454 session not found errors on
        // standard RTSP server implementations.
        // (i.e - sessionId = "DEADBEEFCAFE;timeout = 90") 
        sessionIdString = strdup(strtok(sessionId, ";"));
        if (sessionIdString == NULL) {
            Limelog("Failed to duplicate session ID string\n");
            ret = -1;
            goto Exit;
        }

        hasSessionId = true;

        freeMessage(&response);
    }

    {
        RTSP_MESSAGE response;
        int error = -1;

        if (!setupStream(&response,
                         AppVersionQuad[0] >= 5 ? "streamid=video/0/0" : "streamid=video",
                         &error)) {
            Limelog("RTSP SETUP streamid=video request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP SETUP streamid=video request failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }

        // Parse the video port out of the RTSP SETUP response
        LC_ASSERT(VideoPortNumber == 0);
        if (!parseServerPortFromTransport(&response, &VideoPortNumber)) {
            // Use the well known port if parsing fails
            VideoPortNumber = 47998;

            Limelog("Video port: %u (RTSP parsing failed)\n", VideoPortNumber);
        }
        else {
            Limelog("Video port: %u\n", VideoPortNumber);
        }

        freeMessage(&response);
    }
    
    if (AppVersionQuad[0] >= 5) {
        RTSP_MESSAGE response;
        int error = -1;

        if (!setupStream(&response,
                         controlStreamId,
                         &error)) {
            Limelog("RTSP SETUP streamid=control request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP SETUP streamid=control request failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }

        // Parse the control port out of the RTSP SETUP response
        LC_ASSERT(ControlPortNumber == 0);
        if (!parseServerPortFromTransport(&response, &ControlPortNumber)) {
            // Use the well known port if parsing fails
            ControlPortNumber = 47999;

            Limelog("Control port: %u (RTSP parsing failed)\n", ControlPortNumber);
        }
        else {
            Limelog("Control port: %u\n", ControlPortNumber);
        }

        freeMessage(&response);
    }

    {
        RTSP_MESSAGE response;
        int error = -1;

        if (!sendVideoAnnounce(&response, &error)) {
            Limelog("RTSP ANNOUNCE request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP ANNOUNCE request failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }

        freeMessage(&response);
    }

    // GFE 3.22 uses a single PLAY message
    if (APP_VERSION_AT_LEAST(7, 1, 431)) {
        RTSP_MESSAGE response;
        int error = -1;

        if (!playStream(&response, "/", &error)) {
            Limelog("RTSP PLAY request failed: %d\n", error);
            ret = error;
            goto Exit;
        }

        if (response.message.response.statusCode != 200) {
            Limelog("RTSP PLAY failed: %d\n",
                response.message.response.statusCode);
            ret = response.message.response.statusCode;
            goto Exit;
        }

        freeMessage(&response);
    }
    else {
        {
            RTSP_MESSAGE response;
            int error = -1;

            if (!playStream(&response, "streamid=video", &error)) {
                Limelog("RTSP PLAY streamid=video request failed: %d\n", error);
                ret = error;
                goto Exit;
            }

            if (response.message.response.statusCode != 200) {
                Limelog("RTSP PLAY streamid=video failed: %d\n",
                    response.message.response.statusCode);
                ret = response.message.response.statusCode;
                goto Exit;
            }

            freeMessage(&response);
        }

        {
            RTSP_MESSAGE response;
            int error = -1;

            if (!playStream(&response, "streamid=audio", &error)) {
                Limelog("RTSP PLAY streamid=audio request failed: %d\n", error);
                ret = error;
                goto Exit;
            }

            if (response.message.response.statusCode != 200) {
                Limelog("RTSP PLAY streamid=audio failed: %d\n",
                    response.message.response.statusCode);
                ret = response.message.response.statusCode;
                goto Exit;
            }

            freeMessage(&response);
        }
    }

    
    ret = 0;
    
Exit:
    // Cleanup the ENet stuff
    if (useEnet) {
        if (peer != NULL) {
            enet_peer_disconnect_now(peer, 0);
            peer = NULL;
        }
        
        if (client != NULL) {
            enet_host_destroy(client);
            client = NULL;
        }
    }

    if (sessionIdString != NULL) {
        free(sessionIdString);
        sessionIdString = NULL;
    }

    return ret;
}