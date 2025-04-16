int
repack_data(byte * source, byte * dest, int depth, int first_bit,
                int bit_width, int npixel)
{
    int in_nbyte = depth >> 3;          /* Number of bytes per input pixel */
    int out_nbyte = bit_width >> 3;     /* Number of bytes per output pixel */
    gx_color_index mask = 1;
    gx_color_index data;
    int i, j, length = 0;
    byte temp;
    byte * out = dest;
    int in_bit_start = 8 - depth;
    int out_bit_start = 8 - bit_width;
    int in_byte_loc = in_bit_start, out_byte_loc = out_bit_start;

    mask = (mask << bit_width) - 1;
    for (i=0; i<npixel; i++) {
        /* Get the pixel data */
        if (!in_nbyte) {                /* Multiple pixels per byte */
            data = *source;
            data >>= in_byte_loc;
            in_byte_loc -= depth;
            if (in_byte_loc < 0) {      /* If finished with byte */
                in_byte_loc = in_bit_start;
                source++;
            }
        }
        else {                          /* One or more bytes per pixel */
            data = *source++;
            for (j=1; j<in_nbyte; j++)
                data = (data << 8) + *source++;
        }
        data >>= first_bit;
        data &= mask;

        /* Put the output data */
        if (!out_nbyte) {               /* Multiple pixels per byte */
            temp = (byte)(*out & ~(mask << out_byte_loc));
            *out = (byte)(temp | (data << out_byte_loc));
            out_byte_loc -= bit_width;
            if (out_byte_loc < 0) {     /* If finished with byte */
                out_byte_loc = out_bit_start;
                out++;
            }
        }
        else {                          /* One or more bytes per pixel */
            *out++ = (byte)(data >> ((out_nbyte - 1) * 8));
            for (j=1; j<out_nbyte; j++) {
                *out++ = (byte)(data >> ((out_nbyte - 1 - j) * 8));
            }
        }
    }
    /* Return the number of bytes in the destination buffer. */
    if (out_byte_loc != out_bit_start) {        /* If partially filled last byte */
        *out = *out & ((~0) << out_byte_loc);   /* Mask unused part of last byte */
        out++;
    }
    length = out - dest;
    return length;
}