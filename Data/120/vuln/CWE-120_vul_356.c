static int fillSdpTail(char* buffer) {
    return sprintf(buffer,
        "t=0 0\r\n"
        "m=video %d  \r\n",
        ServerMajorVersion < 4 ? 47996 : 47998);
}