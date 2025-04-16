static inline void
writeStaticConnectResponseHeader(CotpConnection* self, int optionsLength)
{
    /* always same size (7) and same position in buffer */
    uint8_t* buffer = self->writeBuffer->buffer;

    buffer[4] = 6 + optionsLength;
    buffer[5] = 0xd0;
    buffer[6] = (uint8_t) (self->remoteRef / 0x100);
    buffer[7] = (uint8_t) (self->remoteRef & 0xff);
    buffer[8] = (uint8_t) (self->localRef / 0x100);
    buffer[9] = (uint8_t) (self->localRef & 0xff);
    buffer[10] = (uint8_t) (self->protocolClass);

    self->writeBuffer->size = 11;
}