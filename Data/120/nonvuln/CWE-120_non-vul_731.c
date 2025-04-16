static
#endif
    double
    LogL16toY(int p16) /* compute luminance from 16-bit LogL */
{
    int Le = p16 & 0x7fff;
    double Y;

    if (!Le)
        return (0.);
    Y = exp(M_LN2 / 256. * (Le + .5) - M_LN2 * 64.);
    return (!(p16 & 0x8000) ? Y : -Y);
}