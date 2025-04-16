void faad_flushbits_ex(bitfile *ld, uint32_t bits)
{
    uint32_t tmp;

    ld->bufa = ld->bufb;
    if (ld->bytes_left >= 4)
    {
        tmp = getdword(ld->tail);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n(ld->tail, ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;
    ld->tail++;
    ld->bits_left += (32 - bits);
    //ld->bytes_left -= 4;
//    if (ld->bytes_left == 0)
//        ld->no_more_reading = 1;
//    if (ld->bytes_left < 0)
//        ld->error = 1;
}