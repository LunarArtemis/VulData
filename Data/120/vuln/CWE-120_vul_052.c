int ossl_a2ulabel(const char *in, char *out, size_t *outlen)
{
    /*-
     * Domain name has some parts consisting of ASCII chars joined with dot.
     * If a part is shorter than 5 chars, it becomes U-label as is.
     * If it does not start with xn--,    it becomes U-label as is.
     * Otherwise we try to decode it.
     */
    char *outptr = out;
    const char *inptr = in;
    size_t size = 0;
    int result = 1;

    unsigned int buf[LABEL_BUF_SIZE];      /* It's a hostname */
    if (out == NULL)
        result = 0;

    while (1) {
        char *tmpptr = strchr(inptr, '.');
        size_t delta = (tmpptr) ? (size_t)(tmpptr - inptr) : strlen(inptr);

        if (strncmp(inptr, "xn--", 4) != 0) {
            size += delta + 1;

            if (size >= *outlen - 1)
                result = 0;

            if (result > 0) {
                memcpy(outptr, inptr, delta + 1);
                outptr += delta + 1;
            }
        } else {
            unsigned int bufsize = LABEL_BUF_SIZE;
            unsigned int i;

            if (ossl_punycode_decode(inptr + 4, delta - 4, buf, &bufsize) <= 0)
                return -1;

            for (i = 0; i < bufsize; i++) {
                unsigned char seed[6];
                size_t utfsize = codepoint2utf8(seed, buf[i]);
                if (utfsize == 0)
                    return -1;

                size += utfsize;
                if (size >= *outlen - 1)
                    result = 0;

                if (result > 0) {
                    memcpy(outptr, seed, utfsize);
                    outptr += utfsize;
                }
            }

            if (tmpptr != NULL) {
                *outptr = '.';
                outptr++;
                size++;
                if (size >= *outlen - 1)
                    result = 0;
            }
        }

        if (tmpptr == NULL)
            break;

        inptr = tmpptr + 1;
    }

    return result;
}