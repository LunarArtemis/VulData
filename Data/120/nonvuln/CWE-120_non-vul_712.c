static int LogL16Decode(TIFF *tif, uint8_t *op, tmsize_t occ, uint16_t s)
{
    static const char module[] = "LogL16Decode";
    LogLuvState *sp = DecoderState(tif);
    int shft;
    tmsize_t i;
    tmsize_t npixels;
    unsigned char *bp;
    int16_t *tp;
    int16_t b;
    tmsize_t cc;
    int rc;

    (void)s;
    assert(s == 0);
    assert(sp != NULL);

    npixels = occ / sp->pixel_size;

    if (sp->user_datafmt == SGILOGDATAFMT_16BIT)
        tp = (int16_t *)op;
    else
    {
        if (sp->tbuflen < npixels)
        {
            TIFFErrorExtR(tif, module, "Translation buffer too short");
            return (0);
        }
        tp = (int16_t *)sp->tbuf;
    }
    _TIFFmemset((void *)tp, 0, npixels * sizeof(tp[0]));

    bp = (unsigned char *)tif->tif_rawcp;
    cc = tif->tif_rawcc;
    /* get each byte string */
    for (shft = 8; shft >= 0; shft -= 8)
    {
        for (i = 0; i < npixels && cc > 0;)
        {
            if (*bp >= 128)
            { /* run */
                if (cc < 2)
                    break;
                rc = *bp++ + (2 - 128);
                b = (int16_t)(*bp++ << shft);
                cc -= 2;
                while (rc-- && i < npixels)
                    tp[i++] |= b;
            }
            else
            {               /* non-run */
                rc = *bp++; /* nul is noop */
                while (--cc && rc-- && i < npixels)
                    tp[i++] |= (int16_t)*bp++ << shft;
            }
        }
        if (i != npixels)
        {
            TIFFErrorExtR(tif, module,
                          "Not enough data at row %" PRIu32
                          " (short %" TIFF_SSIZE_FORMAT " pixels)",
                          tif->tif_row, npixels - i);
            tif->tif_rawcp = (uint8_t *)bp;
            tif->tif_rawcc = cc;
            return (0);
        }
    }
    (*sp->tfunc)(sp, op, npixels);
    tif->tif_rawcp = (uint8_t *)bp;
    tif->tif_rawcc = cc;
    return (1);
}