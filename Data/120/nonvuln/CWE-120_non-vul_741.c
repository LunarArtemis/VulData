static void L16toGry(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    int16_t *l16 = (int16_t *)sp->tbuf;
    uint8_t *gp = (uint8_t *)op;

    while (n-- > 0)
    {
        double Y = LogL16toY(*l16++);
        *gp++ = (uint8_t)((Y <= 0.)   ? 0
                          : (Y >= 1.) ? 255
                                      : (int)(256. * sqrt(Y)));
    }
}