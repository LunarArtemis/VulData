static
#endif
    uint32_t
    LogLuv32fromXYZ(float *XYZ, int em)
{
    unsigned int Le, ue, ve;
    double u, v, s;
    /* encode luminance */
    Le = (unsigned int)LogL16fromY(XYZ[1], em);
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
    if (u <= 0.)
        ue = 0;
    else
        ue = tiff_itrunc(UVSCALE * u, em);
    if (ue > 255)
        ue = 255;
    if (v <= 0.)
        ve = 0;
    else
        ve = tiff_itrunc(UVSCALE * v, em);
    if (ve > 255)
        ve = 255;
    /* combine encodings */
    return (Le << 16 | ue << 8 | ve);
}