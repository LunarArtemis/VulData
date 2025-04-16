static bool
sendBuffer(CotpConnection* self)
{
    int writeBufferPosition = ByteBuffer_getSize(self->writeBuffer);

    bool retVal = false;

    int sentBytes;

    do {
        sentBytes = writeToSocket(self, ByteBuffer_getBuffer(self->writeBuffer), writeBufferPosition);

        if (sentBytes == -1)
            goto exit_function;

    } while (sentBytes == 0);

    retVal = true;

    ByteBuffer_setSize(self->writeBuffer, 0);

exit_function:
    return retVal;
}