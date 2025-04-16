static int LogLuvSetupEncode(TIFF *tif)
{
    static const char module[] = "LogLuvSetupEncode";
    LogLuvState *sp = EncoderState(tif);
    TIFFDirectory *td = &tif->tif_dir;

    switch (td->td_photometric)
    {
        case PHOTOMETRIC_LOGLUV:
            if (!LogLuvInitState(tif))
                return (0);
            if (td->td_compression == COMPRESSION_SGILOG24)
            {
                tif->tif_encoderow = LogLuvEncode24;
                switch (sp->user_datafmt)
                {
                    case SGILOGDATAFMT_FLOAT:
                        sp->tfunc = Luv24fromXYZ;
                        break;
                    case SGILOGDATAFMT_16BIT:
                        sp->tfunc = Luv24fromLuv48;
                        break;
                    case SGILOGDATAFMT_RAW:
                        break;
                    default:
                        goto notsupported;
                }
            }
            else
            {
                tif->tif_encoderow = LogLuvEncode32;
                switch (sp->user_datafmt)
                {
                    case SGILOGDATAFMT_FLOAT:
                        sp->tfunc = Luv32fromXYZ;
                        break;
                    case SGILOGDATAFMT_16BIT:
                        sp->tfunc = Luv32fromLuv48;
                        break;
                    case SGILOGDATAFMT_RAW:
                        break;
                    default:
                        goto notsupported;
                }
            }
            break;
        case PHOTOMETRIC_LOGL:
            if (!LogL16InitState(tif))
                return (0);
            tif->tif_encoderow = LogL16Encode;
            switch (sp->user_datafmt)
            {
                case SGILOGDATAFMT_FLOAT:
                    sp->tfunc = L16fromY;
                    break;
                case SGILOGDATAFMT_16BIT:
                    break;
                default:
                    goto notsupported;
            }
            break;
        default:
            TIFFErrorExtR(tif, module,
                          "Inappropriate photometric interpretation %" PRIu16
                          " for SGILog compression; %s",
                          td->td_photometric, "must be either LogLUV or LogL");
            return (0);
    }
    sp->encoder_state = 1;
    return (1);
notsupported:
    TIFFErrorExtR(tif, module,
                  "SGILog compression supported only for %s, or raw data",
                  td->td_photometric == PHOTOMETRIC_LOGL ? "Y, L" : "XYZ, Luv");
    return (0);
}