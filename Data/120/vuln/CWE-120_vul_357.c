void processRtpPayload(PNV_VIDEO_PACKET videoPacket, int length) {
    BUFFER_DESC currentPos;
    int frameIndex;
    char flags;
    int firstPacket;
    int streamPacketIndex;

    // Mask the top 8 bits from the SPI
    videoPacket->streamPacketIndex >>= 8;
    videoPacket->streamPacketIndex &= 0xFFFFFF;

    currentPos.data = (char*)(videoPacket + 1);
    currentPos.offset = 0;
    currentPos.length = length - sizeof(*videoPacket);

    frameIndex = videoPacket->frameIndex;
    flags = videoPacket->flags;
    firstPacket = isFirstPacket(flags);

    // Drop duplicates or re-ordered packets
    streamPacketIndex = videoPacket->streamPacketIndex;
    if (isBeforeSignedInt((short)streamPacketIndex, (short)(lastPacketInStream + 1), 0)) {
        return;
    }

    // Drop packets from a previously completed frame
    if (isBeforeSignedInt(frameIndex, nextFrameNumber, 0)) {
        return;
    }

    // Notify the listener of the latest frame we've seen from the PC
    connectionSawFrame(frameIndex);

    // Look for a frame start before receiving a frame end
    if (firstPacket && decodingFrame)
    {
        Limelog("Network dropped end of a frame\n");
        nextFrameNumber = frameIndex;

        // Unexpected start of next frame before terminating the last
        waitingForNextSuccessfulFrame = 1;
        dropFrameState();
    }
    // Look for a non-frame start before a frame start
    else if (!firstPacket && !decodingFrame) {
        // Check if this looks like a real frame
        if (flags == FLAG_CONTAINS_PIC_DATA ||
            flags == FLAG_EOF ||
            currentPos.length < nominalPacketDataLength)
        {
            Limelog("Network dropped beginning of a frame\n");
            nextFrameNumber = frameIndex + 1;

            waitingForNextSuccessfulFrame = 1;

            dropFrameState();
            decodingFrame = 0;
            return;
        }
        else {
            // FEC data
            return;
        }
    }
    // Check sequencing of this frame to ensure we didn't
    // miss one in between
    else if (firstPacket) {
        // Make sure this is the next consecutive frame
        if (isBeforeSignedInt(nextFrameNumber, frameIndex, 1)) {
            Limelog("Network dropped an entire frame\n");
            nextFrameNumber = frameIndex;

            // Wait until next complete frame
            waitingForNextSuccessfulFrame = 1;
            dropFrameState();
        }
        else if (nextFrameNumber != frameIndex) {
            // Duplicate packet or FEC dup
            decodingFrame = 0;
            return;
        }

        // We're now decoding a frame
        decodingFrame = 1;
    }

    // If it's not the first packet of a frame
    // we need to drop it if the stream packet index
    // doesn't match
    if (!firstPacket && decodingFrame) {
        if (streamPacketIndex != (int)(lastPacketInStream + 1)) {
            Limelog("Network dropped middle of a frame\n");
            nextFrameNumber = frameIndex + 1;

            waitingForNextSuccessfulFrame = 1;

            dropFrameState();
            decodingFrame = 0;

            return;
        }
    }

    // Notify the server of any packet losses
    if (streamPacketIndex != (int)(lastPacketInStream + 1)) {
        // Packets were lost so report this to the server
        connectionLostPackets(lastPacketInStream, streamPacketIndex);
    }
    lastPacketInStream = streamPacketIndex;

    // If this is the first packet, skip the frame header (if one exists)
    if (firstPacket && ServerMajorVersion >= 5) {
        currentPos.offset += 8;
        currentPos.length -= 8;
    }

    if (firstPacket && isIdrFrameStart(&currentPos))
    {
        // SPS and PPS prefix is padded between NALs, so we must decode it with the slow path
        processRtpPayloadSlow(videoPacket, &currentPos);
    }
    else
    {
        processRtpPayloadFast(currentPos);
    }

    if (flags & FLAG_EOF) {
        // Move on to the next frame
        decodingFrame = 0;
        nextFrameNumber = frameIndex + 1;

        // If waiting for next successful frame and we got here
        // with an end flag, we can send a message to the server
        if (waitingForNextSuccessfulFrame) {
            // This is the next successful frame after a loss event
            connectionDetectedFrameLoss(startFrameNumber, nextFrameNumber - 1);
            waitingForNextSuccessfulFrame = 0;
        }

        // If we need an IDR frame first, then drop this frame
        if (waitingForIdrFrame) {
            Limelog("Waiting for IDR frame\n");

            dropFrameState();
            return;
        }

        reassembleFrame(frameIndex);

        startFrameNumber = nextFrameNumber;
    }
}