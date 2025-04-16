static int LogLuvInitState(TIFF *tif)
{
    static const char module[] = "LogLuvInitState";
    TIFFDirectory *td = &tif->tif_dir;
    LogLuvState *sp = DecoderState(tif);

    assert(sp != NULL);
    assert(td->td_photometric == PHOTOMETRIC_LOGLUV);

    /* for some reason, we can't do this in TIFFInitLogLuv */
    if (td->td_planarconfig != PLANARCONFIG_CONTIG)
    {
        TIFFErrorExtR(tif, module,
                      "SGILog compression cannot handle non-contiguous data");
        return (0);
    }
    if (sp->user_datafmt == SGILOGDATAFMT_UNKNOWN)
        sp->user_datafmt = LogLuvGuessDataFmt(td);
    switch (sp->user_datafmt)
    {
        case SGILOGDATAFMT_FLOAT:
            sp->pixel_size = 3 * sizeof(float);
            break;
        case SGILOGDATAFMT_16BIT:
            sp->pixel_size = 3 * sizeof(int16_t);
            break;
        case SGILOGDATAFMT_RAW:
            sp->pixel_size = sizeof(uint32_t);
            break;
        case SGILOGDATAFMT_8BIT:
            sp->pixel_size = 3 * sizeof(uint8_t);
            break;
        default:
            TIFFErrorExtR(
                tif, module,
                "No support for converting user data format to LogLuv");
            return (0);
    }
    if (isTiled(tif))
        sp->tbuflen = multiply_ms(td->td_tilewidth, td->td_tilelength);
    else if (td->td_rowsperstrip < td->td_imagelength)
        sp->tbuflen = multiply_ms(td->td_imagewidth, td->td_rowsperstrip);
    else
        sp->tbuflen = multiply_ms(td->td_imagewidth, td->td_imagelength);
    if (multiply_ms(sp->tbuflen, sizeof(uint32_t)) == 0 ||
        (sp->tbuf = (uint8_t *)_TIFFmallocExt(
             tif, sp->tbuflen * sizeof(uint32_t))) == NULL)
    {
        TIFFErrorExtR(tif, module, "No space for SGILog translation buffer");
        return (0);
    }
    return (1);
}