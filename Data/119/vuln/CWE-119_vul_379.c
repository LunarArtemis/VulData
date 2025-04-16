static int base64_encode_base(const void *data_in, size_t data_in_size,
                              unsigned char *base64_out, size_t *base64_out_size,
                              bool urlsafe)
{
    const unsigned char *in = data_in;
    size_t required_size = base64_estimate_encode_size(data_in_size);

    if (data_in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (data_in_size == 0) {
        *base64_out_size = 0;
        return BASE64_SUCCESS;
    }

    if (*base64_out_size < required_size) {
        *base64_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (base64_out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    int iterate_base64_buffer = 0;
    unsigned char nNum = 0;
    int nLst = 0;
    int njump = 0;

    for (int i = 0; i < (int)(data_in_size); ++i) {
        unsigned char tmpval;
        njump++;
        tmpval = *(in + i);

        nNum = (tmpval >> (2 * njump));

        if (njump == 4) {
            nNum = nLst << (8 - 2 * njump);
            njump = 0;
            nLst = 0;
            --i;
        }
        else {
            nNum += nLst << (8 - 2 * njump);
            nLst =  tmpval & ((1 << njump * 2) - 1);
        }

        base64_out[iterate_base64_buffer++] = getsymbol(nNum, urlsafe);
    }

    /* The last character is not finished yet */
    njump++;

    nNum = nLst << (8 - 2 * njump);
    base64_out[iterate_base64_buffer++] = getsymbol(nNum, urlsafe);

    /* if required we append '=' for the required dividability */
    while (iterate_base64_buffer % 4) {
        base64_out[iterate_base64_buffer++] = '=';
    }

    *base64_out_size = iterate_base64_buffer;

    return BASE64_SUCCESS;
}