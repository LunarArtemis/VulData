static bool parseUrlAddrFromRtspUrlString(const char* rtspUrlString, char* destination) {
    char* rtspUrlScratchBuffer;
    char* portSeparator;
    char* v6EscapeEndChar;
    char* urlPathSeparator;
    int prefixLen;

    // Create a copy that we can modify
    rtspUrlScratchBuffer = strdup(rtspUrlString);
    if (rtspUrlScratchBuffer == NULL) {
        return false;
    }

    // If we have a v6 address, we want to stop one character after the closing ]
    // If we have a v4 address, we want to stop at the port separator
    portSeparator = strrchr(rtspUrlScratchBuffer, ':');
    v6EscapeEndChar = strchr(rtspUrlScratchBuffer, ']');

    // Count the prefix length to skip past the initial rtsp:// or rtspru:// part
    for (prefixLen = 2; rtspUrlScratchBuffer[prefixLen - 2] != 0 && (rtspUrlScratchBuffer[prefixLen - 2] != '/' || rtspUrlScratchBuffer[prefixLen - 1] != '/'); prefixLen++);

    // If we hit the end of the string prior to parsing the prefix, we cannot proceed
    if (rtspUrlScratchBuffer[prefixLen - 2] == 0) {
        free(rtspUrlScratchBuffer);
        return false;
    }

    // Look for a slash at the end of the host portion of the URL (may not be present)
    urlPathSeparator = strchr(rtspUrlScratchBuffer + prefixLen, '/');

    // Check for a v6 address first since they also have colons
    if (v6EscapeEndChar) {
        // Terminate the string at the next character
        *(v6EscapeEndChar + 1) = 0;
    }
    else if (portSeparator) {
        // Terminate the string prior to the port separator
        *portSeparator = 0;
    }
    else if (urlPathSeparator) {
        // Terminate the string prior to the path separator
        *urlPathSeparator = 0;
    }

    strcpy(destination, rtspUrlScratchBuffer + prefixLen);

    free(rtspUrlScratchBuffer);
    return true;
}