static inline int
writeToSocket(CotpConnection* self, uint8_t* buf, int size)
{
#if (CONFIG_MMS_SUPPORT_TLS == 1)
    if (self->tlsSocket)
        return TLSSocket_write(self->tlsSocket, buf, size);
    else
        return Socket_write(self->socket, buf, size);
#else
    return Socket_write(self->socket, buf, size);
#endif
}