uint8_t faad_byte_align(bitfile *ld)
{
    int remainder = (32 - ld->bits_left) & 0x7;

    if (remainder)
    {
        faad_flushbits(ld, 8 - remainder);
        return (uint8_t)(8 - remainder);
    }
    return 0;
}