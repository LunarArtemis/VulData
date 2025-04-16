static void Luv24toLuv48(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    uint32_t *luv = (uint32_t *)sp->tbuf;
    int16_t *luv3 = (int16_t *)op;

    while (n-- > 0)
    {
        double u, v;

        *luv3++ = (int16_t)((*luv >> 12 & 0xffd) + 13314);
        if (uv_decode(&u, &v, *luv & 0x3fff) < 0)
        {
            u = U_NEU;
            v = V_NEU;
        }
        *luv3++ = (int16_t)(u * (1L << 15));
        *luv3++ = (int16_t)(v * (1L << 15));
        luv++;
    }
}