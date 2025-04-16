static void Luv24fromXYZ(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    uint32_t *luv = (uint32_t *)sp->tbuf;
    float *xyz = (float *)op;

    while (n-- > 0)
    {
        *luv++ = LogLuv24fromXYZ(xyz, sp->encode_meth);
        xyz += 3;
    }
}