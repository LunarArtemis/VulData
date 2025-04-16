static void Luv24toXYZ(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    uint32_t *luv = (uint32_t *)sp->tbuf;
    float *xyz = (float *)op;

    while (n-- > 0)
    {
        LogLuv24toXYZ(*luv, xyz);
        xyz += 3;
        luv++;
    }
}