static
#endif
    double
    LogL10toY(int p10) /* compute luminance from 10-bit LogL */
{
    if (p10 == 0)
        return (0.);
    return (exp(M_LN2 / 64. * (p10 + .5) - M_LN2 * 12.));
}