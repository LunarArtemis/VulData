void faad_rewindbits(bitfile *ld)
{
    uint32_t tmp;

    ld->bytes_left = ld->buffer_size;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((uint32_t*)&ld->start[0]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((uint32_t*)&ld->start[0], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufa = tmp;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword((uint32_t*)&ld->start[1]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n((uint32_t*)&ld->start[1], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;

    ld->bits_left = 32;
    ld->tail = &ld->start[2];
}