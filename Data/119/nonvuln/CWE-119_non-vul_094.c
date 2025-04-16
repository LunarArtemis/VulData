static inline uint16_t
getUint16(uint8_t* buffer)
{
    return (buffer[0] * 0x100) + buffer[1];
}