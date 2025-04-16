static int LogLuvDecodeTile(TIFF *tif, uint8_t *bp, tmsize_t cc, uint16_t s)
{
    tmsize_t rowlen = TIFFTileRowSize(tif);

    if (rowlen == 0)
        return 0;

    assert(cc % rowlen == 0);
    while (cc && (*tif->tif_decoderow)(tif, bp, rowlen, s))
    {
        bp += rowlen;
        cc -= rowlen;
    }
    return (cc == 0);
}