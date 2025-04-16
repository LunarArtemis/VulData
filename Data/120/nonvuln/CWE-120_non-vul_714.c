static void Luv32fromLuv48(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    uint32_t *luv = (uint32_t *)sp->tbuf;
    int16_t *luv3 = (int16_t *)op;

    if (sp->encode_meth == SGILOGENCODE_NODITHER)
    {
        while (n-- > 0)
        {
            *luv++ = (uint32_t)luv3[0] << 16 |
                     (luv3[1] * (uint32_t)(UVSCALE + .5) >> 7 & 0xff00) |
                     (luv3[2] * (uint32_t)(UVSCALE + .5) >> 15 & 0xff);
            luv3 += 3;
        }
        return;
    }
    while (n-- > 0)
    {
        *luv++ =
            (uint32_t)luv3[0] << 16 |
            (tiff_itrunc(luv3[1] * (UVSCALE / (1 << 15)), sp->encode_meth)
                 << 8 &
             0xff00) |
            (tiff_itrunc(luv3[2] * (UVSCALE / (1 << 15)), sp->encode_meth) &
             0xff);
        luv3 += 3;
    }
}