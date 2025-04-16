int main(int argc, char **argv)
{

    u8 *byteStrmStart;
    u8 *byteStrm;
    u32 strmLen;
    u32 picSize;
    H264SwDecInst decInst;
    H264SwDecRet ret;
    H264SwDecInput decInput;
    H264SwDecOutput decOutput;
    H264SwDecPicture decPicture;
    H264SwDecInfo decInfo;
    u32 picNumber;

    FILE *finput;
    FILE *foutput;

    /* Check that enough command line arguments given, if not -> print usage
     * information out */
    if (argc < 2)
    {
        printf( "Usage: %s file.h264\n", argv[0]);
        return -1;
    }

    /* open output file for writing, output file named out.yuv. If file open
     * fails -> exit */
    foutput = fopen("out.yuv", "wb");
    if (foutput == NULL)
    {
        printf("UNABLE TO OPEN OUTPUT FILE\n");
        return -1;
    }

    /* open input file for reading, file name given by user. If file open
     * fails -> exit */
    finput = fopen(argv[argc-1], "rb");
    if (finput == NULL)
    {
        printf("UNABLE TO OPEN INPUT FILE\n");
        return -1;
    }

    /* check size of the input file -> length of the stream in bytes */
    fseek(finput, 0L, SEEK_END);
    strmLen = (u32)ftell(finput);
    rewind(finput);

    /* allocate memory for stream buffer, exit if unsuccessful */
    byteStrm = byteStrmStart = (u8 *)H264SwDecMalloc(sizeof(u8)*strmLen);
    if (byteStrm == NULL)
    {
        printf("UNABLE TO ALLOCATE MEMORY\n");
        return -1;
    }

    /* read input stream from file to buffer and close input file */
    fread(byteStrm, sizeof(u8), strmLen, finput);
    fclose(finput);

    /* initialize decoder. If unsuccessful -> exit */
    ret = H264SwDecInit(&decInst, 0);
    if (ret != H264SWDEC_OK)
    {
        printf("DECODER INITIALIZATION FAILED\n");
        return -1;
    }

    /* initialize H264SwDecDecode() input structure */
    decInput.pStream = byteStrmStart;
    decInput.dataLen = strmLen;
    decInput.intraConcealmentMethod = 0;

    picNumber = 0;

    /* For performance measurements, read the start time (in seconds) here.
     * The decoding time should be measured over several frames and after
     * that average fps (frames/second) can be calculated.
     *
     * startTime = GetTime();
     *
     * To prevent calculating file I/O latensies as a decoding time,
     * comment out WriteOutput function call. Also prints to stdout might
     * consume considerable amount of cycles during measurement */

    /* main decoding loop */
    do
    {
        /* call API function to perform decoding */
        ret = H264SwDecDecode(decInst, &decInput, &decOutput);

        switch(ret)
        {

            case H264SWDEC_HDRS_RDY_BUFF_NOT_EMPTY:

                /* picture dimensions are available for query now */
                ret = H264SwDecGetInfo(decInst, &decInfo);
                if (ret != H264SWDEC_OK)
                    return -1;

                /* picture size in pixels */
                picSize = decInfo.picWidth * decInfo.picHeight;
                /* memory needed for YCbCr 4:2:0 picture in bytes */
                picSize = (3 * picSize)/2;
                /* memory needed for 16-bit RGB picture in bytes
                 * picSize = (decInfo.picWidth * decInfo.picHeight) * 2; */

                printf("Width %d Height %d\n",
                    decInfo.picWidth, decInfo.picHeight);

                /* update H264SwDecDecode() input structure, number of bytes
                 * "consumed" is computed as difference between the new stream
                 * pointer and old stream pointer */
                decInput.dataLen -=
                    (u32)(decOutput.pStrmCurrPos - decInput.pStream);
                decInput.pStream = decOutput.pStrmCurrPos;
                break;

            case H264SWDEC_PIC_RDY_BUFF_NOT_EMPTY:
            case H264SWDEC_PIC_RDY:

                /* update H264SwDecDecode() input structure, number of bytes
                 * "consumed" is computed as difference between the new stream
                 * pointer and old stream pointer */
                decInput.dataLen -=
                    (u32)(decOutput.pStrmCurrPos - decInput.pStream);
                decInput.pStream = decOutput.pStrmCurrPos;

                /* use function H264SwDecNextPicture() to obtain next picture
                 * in display order. Function is called until no more images
                 * are ready for display */
                while (H264SwDecNextPicture(decInst, &decPicture, 0) ==
                    H264SWDEC_PIC_RDY) { picNumber++;

                    printf("PIC %d, type %s, concealed %d\n", picNumber,
                        decPicture.isIdrPicture ? "IDR" : "NON-IDR",
                        decPicture.nbrOfErrMBs);
                    fflush(stdout);

                    /* Do color conversion if needed to get display image
                     * in RGB-format
                     *
                     * YuvToRgb( decPicture.pOutputPicture, pRgbPicture ); */

                    /* write next display image to output file */
                    WriteOutput(foutput, (u8*)decPicture.pOutputPicture,
                        picSize);
                }

                break;

            case H264SWDEC_EVALUATION_LIMIT_EXCEEDED:
                /* evaluation version of the decoder has limited decoding
                 * capabilities */
                printf("EVALUATION LIMIT REACHED\n");
                goto end;

            default:
                printf("UNRECOVERABLE ERROR\n");
                return -1;
        }
    /* keep decoding until all data from input stream buffer consumed */
    } while (decInput.dataLen > 0);

end:

    /* if output in display order is preferred, the decoder shall be forced
     * to output pictures remaining in decoded picture buffer. Use function
     * H264SwDecNextPicture() to obtain next picture in display order. Function
     * is called until no more images are ready for display. Second parameter
     * for the function is set to '1' to indicate that this is end of the
     * stream and all pictures shall be output */
    while (H264SwDecNextPicture(decInst, &decPicture, 1) ==
        H264SWDEC_PIC_RDY) {

        picNumber++;

        printf("PIC %d, type %s, concealed %d\n", picNumber,
            decPicture.isIdrPicture ? "IDR" : "NON-IDR",
            decPicture.nbrOfErrMBs);
        fflush(stdout);

        /* Do color conversion if needed to get display image
         * in RGB-format
         *
         * YuvToRgb( decPicture.pOutputPicture, pRgbPicture ); */

        /* write next display image to output file */
        WriteOutput(foutput, (u8*)decPicture.pOutputPicture, picSize);
    }

    /* For performance measurements, read the end time (in seconds) here.
     *
     * endTime = GetTime();
     *
     * Now the performance can be calculated as frames per second:
     * fps = picNumber / (endTime - startTime); */


    /* release decoder instance */
    H264SwDecRelease(decInst);

    /* close output file */
    fclose(foutput);

    /* free byte stream buffer */
    free(byteStrmStart);

    return 0;

}