static int sendMessageAndForget(short ptype, short paylen, const void* payload) {
    int ret;

    // Unlike regular sockets, ENet sockets aren't safe to invoke from multiple
    // threads at once. We have to synchronize them with a lock.
    if (ServerMajorVersion >= 5) {
        PltLockMutex(&enetMutex);
        ret = sendMessageEnet(ptype, paylen, payload);
        PltUnlockMutex(&enetMutex);
    }
    else {
        ret = sendMessageTcp(ptype, paylen, payload);
    }

    return ret;
}