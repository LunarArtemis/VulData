int
bpc_to_depth(uchar ncomp, int bpc)
{
    static const byte depths[4][8] = {
        {1, 2, 0, 4, 8, 0, 0, 8},
        {2, 4, 0, 8, 16, 0, 0, 16},
        {4, 8, 0, 16, 16, 0, 0, 24},
        {4, 8, 0, 16, 32, 0, 0, 32}
    };

    if (ncomp <=4 && bpc <= 8)
        return depths[ncomp -1][bpc-1];
    else
        return (ncomp * bpc + 7) & ~7;
}