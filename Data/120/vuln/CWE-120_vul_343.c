int LiStartConnection(PSERVER_INFORMATION serverInfo, PSTREAM_CONFIGURATION streamConfig, PCONNECTION_LISTENER_CALLBACKS clCallbacks,
    PDECODER_RENDERER_CALLBACKS drCallbacks, PAUDIO_RENDERER_CALLBACKS arCallbacks, void* renderContext, int drFlags,
    void* audioContext, int arFlags) {
    int err;

    if (drCallbacks != NULL && (drCallbacks->capabilities & CAPABILITY_PULL_RENDERER) && drCallbacks->submitDecodeUnit) {
        Limelog("CAPABILITY_PULL_RENDERER cannot be set with a submitDecodeUnit callback\n");
        err = -1;
        goto Cleanup;
    }

    if (drCallbacks != NULL && (drCallbacks->capabilities & CAPABILITY_PULL_RENDERER) && (drCallbacks->capabilities & CAPABILITY_DIRECT_SUBMIT)) {
        Limelog("CAPABILITY_PULL_RENDERER and CAPABILITY_DIRECT_SUBMIT cannot be set together\n");
        err = -1;
        goto Cleanup;
    }

    // Replace missing callbacks with placeholders
    fixupMissingCallbacks(&drCallbacks, &arCallbacks, &clCallbacks);
    memcpy(&VideoCallbacks, drCallbacks, sizeof(VideoCallbacks));
    memcpy(&AudioCallbacks, arCallbacks, sizeof(AudioCallbacks));

#ifdef LC_DEBUG_RECORD_MODE
    // Install the pass-through recorder callbacks
    setRecorderCallbacks(&VideoCallbacks, &AudioCallbacks);
#endif

    // Hook the termination callback so we can avoid issuing a termination callback
    // after LiStopConnection() is called.
    //
    // Initialize ListenerCallbacks before anything that could call Limelog().
    originalTerminationCallback = clCallbacks->connectionTerminated;
    memcpy(&ListenerCallbacks, clCallbacks, sizeof(ListenerCallbacks));
    ListenerCallbacks.connectionTerminated = ClInternalConnectionTerminated;

    NegotiatedVideoFormat = 0;
    memcpy(&StreamConfig, streamConfig, sizeof(StreamConfig));
    OriginalVideoBitrate = streamConfig->bitrate;
    RemoteAddrString = strdup(serverInfo->address);

    // The values in RTSP SETUP will be used to populate these.
    VideoPortNumber = 0;
    ControlPortNumber = 0;
    AudioPortNumber = 0;

    // Parse RTSP port number from RTSP session URL
    if (!parseRtspPortNumberFromUrl(serverInfo->rtspSessionUrl, &RtspPortNumber)) {
        // Use the well known port if parsing fails
        RtspPortNumber = 48010;

        Limelog("RTSP port: %u (RTSP URL parsing failed)\n", RtspPortNumber);
    }
    else {
        Limelog("RTSP port: %u\n", RtspPortNumber);
    }

    alreadyTerminated = false;
    ConnectionInterrupted = false;
    
    // Validate the audio configuration
    if (MAGIC_BYTE_FROM_AUDIO_CONFIG(StreamConfig.audioConfiguration) != 0xCA ||
            CHANNEL_COUNT_FROM_AUDIO_CONFIGURATION(StreamConfig.audioConfiguration) > AUDIO_CONFIGURATION_MAX_CHANNEL_COUNT) {
        Limelog("Invalid audio configuration specified\n");
        err = -1;
        goto Cleanup;
    }

    // FEC only works in 16 byte chunks, so we must round down
    // the given packet size to the nearest multiple of 16.
    StreamConfig.packetSize -= StreamConfig.packetSize % 16;

    if (StreamConfig.packetSize == 0) {
        Limelog("Invalid packet size specified\n");
        err = -1;
        goto Cleanup;
    }

    // Height must not be odd or NVENC will fail to initialize
    if (StreamConfig.height & 0x1) {
        Limelog("Encoder height must not be odd. Rounding %d to %d\n",
                StreamConfig.height,
                StreamConfig.height & ~0x1);
        StreamConfig.height = StreamConfig.height & ~0x1;
    }

    // Dimensions over 4096 are only supported with HEVC on NVENC
    if (!StreamConfig.supportsHevc &&
            (StreamConfig.width > 4096 || StreamConfig.height > 4096)) {
        Limelog("WARNING: Streaming at resolutions above 4K using H.264 will likely fail! Trying anyway!\n");
    }
    // Dimensions over 8192 aren't supported at all (even on Turing)
    else if (StreamConfig.width > 8192 || StreamConfig.height > 8192) {
        Limelog("WARNING: Streaming at resolutions above 8K will likely fail! Trying anyway!\n");
    }

    // Reference frame invalidation doesn't seem to work with resolutions much
    // higher than 1440p. I haven't figured out a pattern to indicate which
    // resolutions will work and which won't, but we can at least exclude
    // 4K from RFI to avoid significant persistent artifacts after frame loss.
    if (StreamConfig.width == 3840 && StreamConfig.height == 2160 &&
            (VideoCallbacks.capabilities & CAPABILITY_REFERENCE_FRAME_INVALIDATION_AVC)) {
        Limelog("Disabling reference frame invalidation for 4K streaming\n");
        VideoCallbacks.capabilities &= ~CAPABILITY_REFERENCE_FRAME_INVALIDATION_AVC;
    }
    
    // Extract the appversion from the supplied string
    if (extractVersionQuadFromString(serverInfo->serverInfoAppVersion,
                                     AppVersionQuad) < 0) {
        Limelog("Invalid appversion string: %s\n", serverInfo->serverInfoAppVersion);
        err = -1;
        goto Cleanup;
    }

    Limelog("Initializing platform...");
    ListenerCallbacks.stageStarting(STAGE_PLATFORM_INIT);
    err = initializePlatform();
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_PLATFORM_INIT, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_PLATFORM_INIT);
    ListenerCallbacks.stageComplete(STAGE_PLATFORM_INIT);
    Limelog("done\n");

    Limelog("Resolving host name...");
    ListenerCallbacks.stageStarting(STAGE_NAME_RESOLUTION);
    LC_ASSERT(RtspPortNumber != 0);
    if (RtspPortNumber != 48010) {
        // If we have an alternate RTSP port, use that as our test port. The host probably
        // isn't listening on 47989 or 47984 anyway, since they're using alternate ports.
        err = resolveHostName(serverInfo->address, AF_UNSPEC, RtspPortNumber, &RemoteAddr, &RemoteAddrLen);
        if (err != 0) {
            // Sleep for a second and try again. It's possible that we've attempt to connect
            // before the host has gotten around to listening on the RTSP port. Give it some
            // time before retrying.
            PltSleepMs(1000);
            err = resolveHostName(serverInfo->address, AF_UNSPEC, RtspPortNumber, &RemoteAddr, &RemoteAddrLen);
        }
    }
    else {
        // We use TCP 47984 and 47989 first here because we know those should always be listening
        // on hosts using the standard ports.
        //
        // TCP 48010 is a last resort because:
        // a) it's not always listening and there's a race between listen() on the host and our connect()
        // b) it's not used at all by certain host versions which perform RTSP over ENet
        err = resolveHostName(serverInfo->address, AF_UNSPEC, 47984, &RemoteAddr, &RemoteAddrLen);
        if (err != 0) {
            err = resolveHostName(serverInfo->address, AF_UNSPEC, 47989, &RemoteAddr, &RemoteAddrLen);
        }
        if (err != 0) {
            err = resolveHostName(serverInfo->address, AF_UNSPEC, 48010, &RemoteAddr, &RemoteAddrLen);
        }
    }
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_NAME_RESOLUTION, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_NAME_RESOLUTION);
    ListenerCallbacks.stageComplete(STAGE_NAME_RESOLUTION);
    Limelog("done\n");

    // If STREAM_CFG_AUTO was requested, determine the streamingRemotely value
    // now that we have resolved the target address and impose the video packet
    // size cap if required.
    if (StreamConfig.streamingRemotely == STREAM_CFG_AUTO) {
        if (isPrivateNetworkAddress(&RemoteAddr)) {
            StreamConfig.streamingRemotely = STREAM_CFG_LOCAL;
        }
        else {
            StreamConfig.streamingRemotely = STREAM_CFG_REMOTE;

            if (StreamConfig.packetSize > 1024) {
                // Cap packet size at 1024 for remote streaming to avoid
                // MTU problems and fragmentation.
                Limelog("Packet size capped at 1KB for remote streaming\n");
                StreamConfig.packetSize = 1024;
            }
        }
    }

    Limelog("Initializing audio stream...");
    ListenerCallbacks.stageStarting(STAGE_AUDIO_STREAM_INIT);
    err = initializeAudioStream();
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_AUDIO_STREAM_INIT, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_AUDIO_STREAM_INIT);
    ListenerCallbacks.stageComplete(STAGE_AUDIO_STREAM_INIT);
    Limelog("done\n");

    Limelog("Starting RTSP handshake...");
    ListenerCallbacks.stageStarting(STAGE_RTSP_HANDSHAKE);
    err = performRtspHandshake();
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_RTSP_HANDSHAKE, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_RTSP_HANDSHAKE);
    ListenerCallbacks.stageComplete(STAGE_RTSP_HANDSHAKE);
    Limelog("done\n");

    Limelog("Initializing control stream...");
    ListenerCallbacks.stageStarting(STAGE_CONTROL_STREAM_INIT);
    err = initializeControlStream();
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_CONTROL_STREAM_INIT, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_CONTROL_STREAM_INIT);
    ListenerCallbacks.stageComplete(STAGE_CONTROL_STREAM_INIT);
    Limelog("done\n");

    Limelog("Initializing video stream...");
    ListenerCallbacks.stageStarting(STAGE_VIDEO_STREAM_INIT);
    initializeVideoStream();
    stage++;
    LC_ASSERT(stage == STAGE_VIDEO_STREAM_INIT);
    ListenerCallbacks.stageComplete(STAGE_VIDEO_STREAM_INIT);
    Limelog("done\n");

    Limelog("Initializing input stream...");
    ListenerCallbacks.stageStarting(STAGE_INPUT_STREAM_INIT);
    initializeInputStream();
    stage++;
    LC_ASSERT(stage == STAGE_INPUT_STREAM_INIT);
    ListenerCallbacks.stageComplete(STAGE_INPUT_STREAM_INIT);
    Limelog("done\n");

    Limelog("Starting control stream...");
    ListenerCallbacks.stageStarting(STAGE_CONTROL_STREAM_START);
    err = startControlStream();
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_CONTROL_STREAM_START, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_CONTROL_STREAM_START);
    ListenerCallbacks.stageComplete(STAGE_CONTROL_STREAM_START);
    Limelog("done\n");

    Limelog("Starting video stream...");
    ListenerCallbacks.stageStarting(STAGE_VIDEO_STREAM_START);
    err = startVideoStream(renderContext, drFlags);
    if (err != 0) {
        Limelog("Video stream start failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_VIDEO_STREAM_START, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_VIDEO_STREAM_START);
    ListenerCallbacks.stageComplete(STAGE_VIDEO_STREAM_START);
    Limelog("done\n");

    Limelog("Starting audio stream...");
    ListenerCallbacks.stageStarting(STAGE_AUDIO_STREAM_START);
    err = startAudioStream(audioContext, arFlags);
    if (err != 0) {
        Limelog("Audio stream start failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_AUDIO_STREAM_START, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_AUDIO_STREAM_START);
    ListenerCallbacks.stageComplete(STAGE_AUDIO_STREAM_START);
    Limelog("done\n");

    Limelog("Starting input stream...");
    ListenerCallbacks.stageStarting(STAGE_INPUT_STREAM_START);
    err = startInputStream();
    if (err != 0) {
        Limelog("Input stream start failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_INPUT_STREAM_START, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_INPUT_STREAM_START);
    ListenerCallbacks.stageComplete(STAGE_INPUT_STREAM_START);
    Limelog("done\n");
    
    // Wiggle the mouse a bit to wake the display up
    LiSendMouseMoveEvent(1, 1);
    PltSleepMs(10);
    LiSendMouseMoveEvent(-1, -1);
    PltSleepMs(10);

    ListenerCallbacks.connectionStarted();

Cleanup:
    if (err != 0) {
        // Undo any work we've done here before failing
        LiStopConnection();
    }
    return err;
}