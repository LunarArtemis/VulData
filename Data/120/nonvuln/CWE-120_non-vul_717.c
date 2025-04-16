static
#endif
    int
    LogL10fromY(double Y, int em) /* get 10-bit LogL from Y */
{
    if (Y >= 15.742)
        return (0x3ff);
    else if (Y <= .00024283)
        return (0);
    else
        return tiff_itrunc(64. * (log2(Y) + 12.), em);
}