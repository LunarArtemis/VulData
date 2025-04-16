uint8_t *faad_getbitbuffer(bitfile *ld, uint32_t bits
                       DEBUGDEC)
{
    int i;
    unsigned int temp;
    int bytes = bits >> 3;
    int remainder = bits & 0x7;

    uint8_t *buffer = (uint8_t*)faad_malloc((bytes+1)*sizeof(uint8_t));

    for (i = 0; i < bytes; i++)
    {
        buffer[i] = (uint8_t)faad_getbits(ld, 8 DEBUGVAR(print,var,dbg));
    }

    if (remainder)
    {
        temp = faad_getbits(ld, remainder DEBUGVAR(print,var,dbg)) << (8-remainder);

        buffer[bytes] = (uint8_t)temp;
    }

    return buffer;
}