int LiStartConnection(const char* host, PSTREAM_CONFIGURATION streamConfig, PCONNECTION_LISTENER_CALLBACKS clCallbacks,
    PDECODER_RENDERER_CALLBACKS drCallbacks, PAUDIO_RENDERER_CALLBACKS arCallbacks,
    void* renderContext, int drFlags, int _serverMajorVersion) {
    int err;

    NegotiatedVideoFormat = 0;
    ServerMajorVersion = _serverMajorVersion;
    memcpy(&StreamConfig, streamConfig, sizeof(StreamConfig));
    RemoteAddrString = strdup(host);

    // Replace missing callbacks with placeholders
    fixupMissingCallbacks(&drCallbacks, &arCallbacks, &clCallbacks);
    memcpy(&VideoCallbacks, drCallbacks, sizeof(VideoCallbacks));
    memcpy(&AudioCallbacks, arCallbacks, sizeof(AudioCallbacks));

    // Hook the termination callback so we can avoid issuing a termination callback
    // after LiStopConnection() is called
    originalTerminationCallback = clCallbacks->connectionTerminated;
    memcpy(&ListenerCallbacks, clCallbacks, sizeof(ListenerCallbacks));
    ListenerCallbacks.connectionTerminated = ClInternalConnectionTerminated;

    alreadyTerminated = 0;

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
    err = resolveHostName(host);
    if (err != 0) {
        Limelog("failed: %d\n", err);
        ListenerCallbacks.stageFailed(STAGE_NAME_RESOLUTION, err);
        goto Cleanup;
    }
    stage++;
    LC_ASSERT(stage == STAGE_NAME_RESOLUTION);
    ListenerCallbacks.stageComplete(STAGE_NAME_RESOLUTION);
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

    Limelog("Initializing audio stream...");
    ListenerCallbacks.stageStarting(STAGE_AUDIO_STREAM_INIT);
    initializeAudioStream();
    stage++;
    LC_ASSERT(stage == STAGE_AUDIO_STREAM_INIT);
    ListenerCallbacks.stageComplete(STAGE_AUDIO_STREAM_INIT);
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
    err = startAudioStream();
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

    ListenerCallbacks.connectionStarted();

Cleanup:
    return err;
}