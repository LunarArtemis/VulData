static
#endif
    void
    LogLuv24toXYZ(uint32_t p, float *XYZ)
{
    int Ce;
    double L, u, v, s, x, y;
    /* decode luminance */
    L = LogL10toY(p >> 14 & 0x3ff);
    if (L <= 0.)
    {
        XYZ[0] = XYZ[1] = XYZ[2] = 0.;
        return;
    }
    /* decode color */
    Ce = p & 0x3fff;
    if (uv_decode(&u, &v, Ce) < 0)
    {
        u = U_NEU;
        v = V_NEU;
    }
    s = 1. / (6. * u - 16. * v + 12.);
    x = 9. * u * s;
    y = 4. * v * s;
    /* convert to XYZ */
    XYZ[0] = (float)(x / y * L);
    XYZ[1] = (float)L;
    XYZ[2] = (float)((1. - x - y) / y * L);
}