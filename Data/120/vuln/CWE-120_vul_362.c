int initializeControlStream(void) {
    stopping = 0;
    PltCreateEvent(&invalidateRefFramesEvent);
    LbqInitializeLinkedBlockingQueue(&invalidReferenceFrameTuples, 20);

    if (ServerMajorVersion == 3) {
        packetTypes = (short*)packetTypesGen3;
        payloadLengths = (short*)payloadLengthsGen3;
        preconstructedPayloads = (char**)preconstructedPayloadsGen3;
    }
    else if (ServerMajorVersion == 4) {
        packetTypes = (short*)packetTypesGen4;
        payloadLengths = (short*)payloadLengthsGen4;
        preconstructedPayloads = (char**)preconstructedPayloadsGen4;
    }
    else if (ServerMajorVersion == 5) {
        packetTypes = (short*)packetTypesGen5;
        payloadLengths = (short*)payloadLengthsGen5;
        preconstructedPayloads = (char**)preconstructedPayloadsGen5;
    }
    else {
        packetTypes = (short*)packetTypesGen7;
        payloadLengths = (short*)payloadLengthsGen7;
        preconstructedPayloads = (char**)preconstructedPayloadsGen7;
    }

    idrFrameRequired = 0;
    lastGoodFrame = 0;
    lastSeenFrame = 0;
    lossCountSinceLastReport = 0;

    return 0;
}