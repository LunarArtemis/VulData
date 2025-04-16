static int LogLuvGuessDataFmt(TIFFDirectory *td)
{
    int guess;

    /*
     * If the user didn't tell us their datafmt,
     * take our best guess from the bitspersample.
     */
#define PACK(a, b) (((a) << 3) | (b))
    switch (PACK(td->td_bitspersample, td->td_sampleformat))
    {
        case PACK(32, SAMPLEFORMAT_IEEEFP):
            guess = SGILOGDATAFMT_FLOAT;
            break;
        case PACK(32, SAMPLEFORMAT_VOID):
        case PACK(32, SAMPLEFORMAT_UINT):
        case PACK(32, SAMPLEFORMAT_INT):
            guess = SGILOGDATAFMT_RAW;
            break;
        case PACK(16, SAMPLEFORMAT_VOID):
        case PACK(16, SAMPLEFORMAT_INT):
        case PACK(16, SAMPLEFORMAT_UINT):
            guess = SGILOGDATAFMT_16BIT;
            break;
        case PACK(8, SAMPLEFORMAT_VOID):
        case PACK(8, SAMPLEFORMAT_UINT):
            guess = SGILOGDATAFMT_8BIT;
            break;
        default:
            guess = SGILOGDATAFMT_UNKNOWN;
            break;
#undef PACK
    }
    /*
     * Double-check samples per pixel.
     */
    switch (td->td_samplesperpixel)
    {
        case 1:
            if (guess != SGILOGDATAFMT_RAW)
                guess = SGILOGDATAFMT_UNKNOWN;
            break;
        case 3:
            if (guess == SGILOGDATAFMT_RAW)
                guess = SGILOGDATAFMT_UNKNOWN;
            break;
        default:
            guess = SGILOGDATAFMT_UNKNOWN;
            break;
    }
    return (guess);
}