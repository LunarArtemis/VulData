static void LogLuvCleanup(TIFF *tif)
{
    LogLuvState *sp = (LogLuvState *)tif->tif_data;

    assert(sp != 0);

    tif->tif_tagmethods.vgetfield = sp->vgetparent;
    tif->tif_tagmethods.vsetfield = sp->vsetparent;

    if (sp->tbuf)
        _TIFFfreeExt(tif, sp->tbuf);
    _TIFFfreeExt(tif, sp);
    tif->tif_data = NULL;

    _TIFFSetDefaultCompressionState(tif);
}