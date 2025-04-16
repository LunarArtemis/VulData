void faad_initbits_rev(bitfile *ld, void *buffer,
                       uint32_t bits_in_buffer)
{
    uint32_t tmp;
    int32_t index;

    ld->buffer_size = bit2byte(bits_in_buffer);

    index = (bits_in_buffer+31)/32 - 1;

    ld->start = (uint32_t*)buffer + index - 2;

    tmp = getdword((uint32_t*)buffer + index);
    ld->bufa = tmp;

    tmp = getdword((uint32_t*)buffer + index - 1);
    ld->bufb = tmp;

    ld->tail = (uint32_t*)buffer + index;

    ld->bits_left = bits_in_buffer % 32;
    if (ld->bits_left == 0)
        ld->bits_left = 32;

    ld->bytes_left = ld->buffer_size;
    ld->error = 0;
}