int base64_decode(const unsigned char *base64_in, size_t base64_in_size,
                  void *data_out, size_t *data_out_size)
{
    unsigned char *out = data_out;
    size_t required_size = base64_estimate_decode_size(base64_in_size);

    if (base64_in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (base64_in_size == 0) {
        *data_out_size = 0;
        return BASE64_SUCCESS;
    }

    if (base64_in_size < 4) {
        return BASE64_ERROR_DATA_IN_SIZE;
    }

    if (*data_out_size < required_size) {
        *data_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (data_out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    int iterate_data_buffer = 0;
    unsigned char nNum = 0;
    int nLst = getcode(base64_in[0]) << 2;
    int code = 0;

    int mask = 2;

    for (int i = 1; i < (int)(base64_in_size); i++) {
        code = getcode(base64_in[i]);

        if (code == BASE64_NOT_DEFINED || code == BASE64_EQUALS) {
            continue;
        }

        int nm = (0xFF << (2 * mask));

        nNum = nLst + ((code & (0xFF & nm)) >> (2 * mask));
        nLst = (code & (0xFF & ~nm)) << (8 - (2 * mask));

        (mask != 3) ? out[iterate_data_buffer++] = nNum : nNum;
        (mask == 0) ? mask = 3 : mask--;
    }

    if (code == BASE64_EQUALS) {
        /* add the last character to the data_out buffer */
        out[iterate_data_buffer] = nNum;
    }

    *data_out_size = iterate_data_buffer;
    return BASE64_SUCCESS;
}