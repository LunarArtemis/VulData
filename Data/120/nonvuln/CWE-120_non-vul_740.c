static void Luv32toRGB(LogLuvState *sp, uint8_t *op, tmsize_t n)
{
    uint32_t *luv = (uint32_t *)sp->tbuf;
    uint8_t *rgb = (uint8_t *)op;

    while (n-- > 0)
    {
        float xyz[3];

        LogLuv32toXYZ(*luv++, xyz);
        XYZtoRGB24(xyz, rgb);
        rgb += 3;
    }
}