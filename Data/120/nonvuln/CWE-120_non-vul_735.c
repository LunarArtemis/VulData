static void Luv32toLuv48(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    uint32_t *luv = (uint32_t *)sp->tbuf;
    int16_t *luv3 = (int16_t *)op;

    while (n-- > 0)
    {
        double u, v;

        *luv3++ = (int16_t)(*luv >> 16);
        u = 1. / UVSCALE * ((*luv >> 8 & 0xff) + .5);
        v = 1. / UVSCALE * ((*luv & 0xff) + .5);
        *luv3++ = (int16_t)(u * (1L << 15));
        *luv3++ = (int16_t)(v * (1L << 15));
        luv++;
    }
}