static int LogL16GuessDataFmt(TIFFDirectory *td)
{
#define PACK(s, b, f) (((b) << 6) | ((s) << 3) | (f))
    switch (
        PACK(td->td_samplesperpixel, td->td_bitspersample, td->td_sampleformat))
    {
        case PACK(1, 32, SAMPLEFORMAT_IEEEFP):
            return (SGILOGDATAFMT_FLOAT);
        case PACK(1, 16, SAMPLEFORMAT_VOID):
        case PACK(1, 16, SAMPLEFORMAT_INT):
        case PACK(1, 16, SAMPLEFORMAT_UINT):
            return (SGILOGDATAFMT_16BIT);
        case PACK(1, 8, SAMPLEFORMAT_VOID):
        case PACK(1, 8, SAMPLEFORMAT_UINT):
            return (SGILOGDATAFMT_8BIT);
    }
#undef PACK
    return (SGILOGDATAFMT_UNKNOWN);
}