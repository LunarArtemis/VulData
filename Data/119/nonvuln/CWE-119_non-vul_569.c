void faad_initbits(bitfile *ld, const void *_buffer, const uint32_t buffer_size)
{
    uint32_t tmp;

    if (ld == NULL)
        return;

    // useless
    //memset(ld, 0, sizeof(bitfile));

    if (buffer_size == 0 || _buffer == NULL)
    {
        ld->error = 1;
        return;
    }

    ld->buffer = _buffer;

    ld->buffer_size = buffer_size;
    ld->bytes_left  = buffer_size;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((uint32_t*)ld->buffer);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((uint32_t*)ld->buffer, ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufa = tmp;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((uint32_t*)ld->buffer + 1);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((uint32_t*)ld->buffer + 1, ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;

    ld->start = (uint32_t*)ld->buffer;
    ld->tail = ((uint32_t*)ld->buffer + 2);

    ld->bits_left = 32;

    ld->error = 0;
}