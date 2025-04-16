static int LogL16InitState(TIFF *tif)
{
    static const char module[] = "LogL16InitState";
    TIFFDirectory *td = &tif->tif_dir;
    LogLuvState *sp = DecoderState(tif);

    assert(sp != NULL);
    assert(td->td_photometric == PHOTOMETRIC_LOGL);

    if (td->td_samplesperpixel != 1)
    {
        TIFFErrorExtR(tif, module,
                      "Sorry, can not handle LogL image with %s=%" PRIu16,
                      "Samples/pixel", td->td_samplesperpixel);
        return 0;
    }

    /* for some reason, we can't do this in TIFFInitLogL16 */
    if (sp->user_datafmt == SGILOGDATAFMT_UNKNOWN)
        sp->user_datafmt = LogL16GuessDataFmt(td);
    switch (sp->user_datafmt)
    {
        case SGILOGDATAFMT_FLOAT:
            sp->pixel_size = sizeof(float);
            break;
        case SGILOGDATAFMT_16BIT:
            sp->pixel_size = sizeof(int16_t);
            break;
        case SGILOGDATAFMT_8BIT:
            sp->pixel_size = sizeof(uint8_t);
            break;
        default:
            TIFFErrorExtR(tif, module,
                          "No support for converting user data format to LogL");
            return (0);
    }
    if (isTiled(tif))
        sp->tbuflen = multiply_ms(td->td_tilewidth, td->td_tilelength);
    else if (td->td_rowsperstrip < td->td_imagelength)
        sp->tbuflen = multiply_ms(td->td_imagewidth, td->td_rowsperstrip);
    else
        sp->tbuflen = multiply_ms(td->td_imagewidth, td->td_imagelength);
    if (multiply_ms(sp->tbuflen, sizeof(int16_t)) == 0 ||
        (sp->tbuf = (uint8_t *)_TIFFmallocExt(
             tif, sp->tbuflen * sizeof(int16_t))) == NULL)
    {
        TIFFErrorExtR(tif, module, "No space for SGILog translation buffer");
        return (0);
    }
    return (1);
}