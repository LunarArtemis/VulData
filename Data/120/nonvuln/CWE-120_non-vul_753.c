static void L16fromY(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    int16_t *l16 = (int16_t *)sp->tbuf;
    float *yp = (float *)op;

    while (n-- > 0)
        *l16++ = (int16_t)(LogL16fromY(*yp++, sp->encode_meth));
}