static int tiff_itrunc(double x, int m)
{
    if (m == SGILOGENCODE_NODITHER)
        return (int)x;
    /* Silence CoverityScan warning about bad crypto function */
    /* coverity[dont_call] */
    return (int)(x + rand() * (1. / RAND_MAX) - .5);
}