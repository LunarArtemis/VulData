static PSDP_OPTION getAttributesList(char*urlSafeAddr) {
    PSDP_OPTION optionHead;
    char payloadStr[92];
    int audioChannelCount;
    int audioChannelMask;
    int err;

    optionHead = NULL;
    err = 0;

    sprintf(payloadStr, "%d", StreamConfig.width);
    err |= addAttributeString(&optionHead, "x-nv-video[0].clientViewportWd", payloadStr);
    sprintf(payloadStr, "%d", StreamConfig.height);
    err |= addAttributeString(&optionHead, "x-nv-video[0].clientViewportHt", payloadStr);

    sprintf(payloadStr, "%d", StreamConfig.fps);
    err |= addAttributeString(&optionHead, "x-nv-video[0].maxFPS", payloadStr);

    sprintf(payloadStr, "%d", StreamConfig.packetSize);
    err |= addAttributeString(&optionHead, "x-nv-video[0].packetSize", payloadStr);

    err |= addAttributeString(&optionHead, "x-nv-video[0].rateControlMode", "4");

    err |= addAttributeString(&optionHead, "x-nv-video[0].timeoutLengthMs", "7000");
    err |= addAttributeString(&optionHead, "x-nv-video[0].framesWithInvalidRefThreshold", "0");

    sprintf(payloadStr, "%d", StreamConfig.bitrate);
    if (ServerMajorVersion >= 5) {
        err |= addAttributeString(&optionHead, "x-nv-vqos[0].bw.minimumBitrateKbps", payloadStr);
        err |= addAttributeString(&optionHead, "x-nv-vqos[0].bw.maximumBitrateKbps", payloadStr);
    }
    else {
        if (StreamConfig.streamingRemotely) {
            err |= addAttributeString(&optionHead, "x-nv-video[0].averageBitrate", "4");
            err |= addAttributeString(&optionHead, "x-nv-video[0].peakBitrate", "4");
        }
        // We don't support dynamic bitrate scaling properly (it tends to bounce between min and max and never
        // settle on the optimal bitrate if it's somewhere in the middle), so we'll just latch the bitrate
        // to the requested value.

        err |= addAttributeString(&optionHead, "x-nv-vqos[0].bw.minimumBitrate", payloadStr);
        err |= addAttributeString(&optionHead, "x-nv-vqos[0].bw.maximumBitrate", payloadStr);
    }

    // Using FEC turns padding on which makes us have to take the slow path
    // in the depacketizer, not to mention exposing some ambiguous cases with
    // distinguishing padding from valid sequences. Since we can only perform
    // execute an FEC recovery on a 1 packet frame, we'll just turn it off completely.
    err |= addAttributeString(&optionHead, "x-nv-vqos[0].fec.enable", "0");

    err |= addAttributeString(&optionHead, "x-nv-vqos[0].videoQualityScoreUpdateTime", "5000");

    if (StreamConfig.streamingRemotely) {
        err |= addAttributeString(&optionHead, "x-nv-vqos[0].qosTrafficType", "0");
        err |= addAttributeString(&optionHead, "x-nv-aqos.qosTrafficType", "0");
    }
    else {
        err |= addAttributeString(&optionHead, "x-nv-vqos[0].qosTrafficType", "5");
        err |= addAttributeString(&optionHead, "x-nv-aqos.qosTrafficType", "4");
    }

    if (ServerMajorVersion == 3) {
        err |= addGen3Options(&optionHead, urlSafeAddr);
    }
    else if (ServerMajorVersion == 4) {
        err |= addGen4Options(&optionHead, urlSafeAddr);
    }
    else {
        err |= addGen5Options(&optionHead);
    }

    if (ServerMajorVersion >= 4) {
        if (NegotiatedVideoFormat == VIDEO_FORMAT_H265) {
            err |= addAttributeString(&optionHead, "x-nv-clientSupportHevc", "1");
            err |= addAttributeString(&optionHead, "x-nv-vqos[0].bitStreamFormat", "1");
            
            // Disable slicing on HEVC
            err |= addAttributeString(&optionHead, "x-nv-video[0].videoEncoderSlicesPerFrame", "1");
        }
        else {
            unsigned char slicesPerFrame;
            
            err |= addAttributeString(&optionHead, "x-nv-clientSupportHevc", "0");
            err |= addAttributeString(&optionHead, "x-nv-vqos[0].bitStreamFormat", "0");
            
            // Use slicing for increased performance on some decoders
            slicesPerFrame = (unsigned char)(VideoCallbacks.capabilities >> 24);
            if (slicesPerFrame == 0) {
                // If not using slicing, we request 1 slice per frame
                slicesPerFrame = 1;
            }
            sprintf(payloadStr, "%d", slicesPerFrame);
            err |= addAttributeString(&optionHead, "x-nv-video[0].videoEncoderSlicesPerFrame", payloadStr);
        }
        
        if (StreamConfig.audioConfiguration == AUDIO_CONFIGURATION_51_SURROUND) {
            audioChannelCount = CHANNEL_COUNT_51_SURROUND;
            audioChannelMask = CHANNEL_MASK_51_SURROUND;
        }
        else {
            audioChannelCount = CHANNEL_COUNT_STEREO;
            audioChannelMask = CHANNEL_MASK_STEREO;
        }

        sprintf(payloadStr, "%d", audioChannelCount);
        err |= addAttributeString(&optionHead, "x-nv-audio.surround.numChannels", payloadStr);
        sprintf(payloadStr, "%d", audioChannelMask);
        err |= addAttributeString(&optionHead, "x-nv-audio.surround.channelMask", payloadStr);
        if (audioChannelCount > 2) {
            err |= addAttributeString(&optionHead, "x-nv-audio.surround.enable", "1");
        }
        else {
            err |= addAttributeString(&optionHead, "x-nv-audio.surround.enable", "0");
        }
    }

    if (err == 0) {
        return optionHead;
    }

    freeAttributeList(optionHead);
    return NULL;
}