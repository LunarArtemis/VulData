int TIFFInitSGILog(TIFF *tif, int scheme)
{
    static const char module[] = "TIFFInitSGILog";
    LogLuvState *sp;

    assert(scheme == COMPRESSION_SGILOG24 || scheme == COMPRESSION_SGILOG);

    /*
     * Merge codec-specific tag information.
     */
    if (!_TIFFMergeFields(tif, LogLuvFields, TIFFArrayCount(LogLuvFields)))
    {
        TIFFErrorExtR(tif, module, "Merging SGILog codec-specific tags failed");
        return 0;
    }

    /*
     * Allocate state block so tag methods have storage to record values.
     */
    tif->tif_data = (uint8_t *)_TIFFmallocExt(tif, sizeof(LogLuvState));
    if (tif->tif_data == NULL)
        goto bad;
    sp = (LogLuvState *)tif->tif_data;
    _TIFFmemset((void *)sp, 0, sizeof(*sp));
    sp->user_datafmt = SGILOGDATAFMT_UNKNOWN;
    sp->encode_meth = (scheme == COMPRESSION_SGILOG24) ? SGILOGENCODE_RANDITHER
                                                       : SGILOGENCODE_NODITHER;
    sp->tfunc = _logLuvNop;

    /*
     * Install codec methods.
     * NB: tif_decoderow & tif_encoderow are filled
     *     in at setup time.
     */
    tif->tif_fixuptags = LogLuvFixupTags;
    tif->tif_setupdecode = LogLuvSetupDecode;
    tif->tif_decodestrip = LogLuvDecodeStrip;
    tif->tif_decodetile = LogLuvDecodeTile;
    tif->tif_setupencode = LogLuvSetupEncode;
    tif->tif_encodestrip = LogLuvEncodeStrip;
    tif->tif_encodetile = LogLuvEncodeTile;
    tif->tif_close = LogLuvClose;
    tif->tif_cleanup = LogLuvCleanup;

    /*
     * Override parent get/set field methods.
     */
    sp->vgetparent = tif->tif_tagmethods.vgetfield;
    tif->tif_tagmethods.vgetfield = LogLuvVGetField; /* hook for codec tags */
    sp->vsetparent = tif->tif_tagmethods.vsetfield;
    tif->tif_tagmethods.vsetfield = LogLuvVSetField; /* hook for codec tags */

    return (1);
bad:
    TIFFErrorExtR(tif, module, "%s: No space for LogLuv state block",
                  tif->tif_name);
    return (0);
}