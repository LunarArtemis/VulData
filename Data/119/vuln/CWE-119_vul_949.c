static void parse_outhdr_string(const guchar *outhdr_string, gint outhdr_string_len)
{
    int   n                 = 0;

    /* Populate values array */
    for (outhdr_values_found=0; outhdr_values_found < MAX_OUTHDR_VALUES; ) {

        guint  digit_array[MAX_OUTHDR_VALUES];
        guint  number_digits = 0;

        guint   number = 0;
        guint   multiplier = 1;
        guint   d;

        /* Find digits */
        for ( ; n < outhdr_string_len; n++) {
            if (!g_ascii_isdigit(outhdr_string[n])) {
                break;
            }
            else {
                digit_array[number_digits++] = outhdr_string[n] - '0';
            }
        }

        if (number_digits == 0) {
            /* No more numbers left */
            break;
        }

        /* Convert digits into value (much faster than format_text() + atoi()) */
        for (d=number_digits; d > 0; d--) {
            number += ((digit_array[d-1]) * multiplier);
            multiplier *= 10;
        }
        outhdr_values[outhdr_values_found++] = number;

        /* Skip comma */
        n++;
    }
}