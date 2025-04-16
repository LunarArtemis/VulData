static int
compress1_row(const byte *row, const byte *end_row,
  byte *compressed)
{	register const byte *in = row;
        register byte *out = compressed;
        while ( in < end_row )
           {	byte test = *in++;
                const byte *run = in;
                while ( in < end_row && *in == test ) in++;
                /* Note that in - run + 1 is the repetition count. */
                while ( in - run > 255 )
                   {	*out++ = 255;
                        *out++ = ~test;
                        run += 256;
                   }
                *out++ = in - run;
                *out++ = ~test;
           }
        return out - compressed;
}