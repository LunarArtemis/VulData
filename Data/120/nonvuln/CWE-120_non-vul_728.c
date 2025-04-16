static
#endif
    int
    LogL16fromY(double Y, int em) /* get 16-bit LogL from Y */
{
    if (Y >= 1.8371976e19)
        return (0x7fff);
    if (Y <= -1.8371976e19)
        return (0xffff);
    if (Y > 5.4136769e-20)
        return tiff_itrunc(256. * (log2(Y) + 64.), em);
    if (Y < -5.4136769e-20)
        return (~0x7fff | tiff_itrunc(256. * (log2(-Y) + 64.), em));
    return (0);
}