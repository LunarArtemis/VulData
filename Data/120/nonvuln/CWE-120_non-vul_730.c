static
#endif
    uint32_t
    LogLuv24fromXYZ(float *XYZ, int em)
{
    int Le, Ce;
    double u, v, s;
    /* encode luminance */
    Le = LogL10fromY(XYZ[1], em);
    /* encode color */
    s = XYZ[0] + 15. * XYZ[1] + 3. * XYZ[2];
    if (!Le || s <= 0.)
    {
        u = U_NEU;
        v = V_NEU;
    }
    else
    {
        u = 4. * XYZ[0] / s;
        v = 9. * XYZ[1] / s;
    }
    Ce = uv_encode(u, v, em);
    if (Ce < 0) /* never happens */
        Ce = uv_encode(U_NEU, V_NEU, SGILOGENCODE_NODITHER);
    /* combine encodings */
    return (Le << 14 | Ce);
}