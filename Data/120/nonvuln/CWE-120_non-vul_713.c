static void L16toY(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    int16_t *l16 = (int16_t *)sp->tbuf;
    float *yp = (float *)op;

    while (n-- > 0)
        *yp++ = (float)LogL16toY(*l16++);
}