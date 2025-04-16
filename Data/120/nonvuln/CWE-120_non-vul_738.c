static int oog_encode(double u, double v) /* encode out-of-gamut chroma */
{
    static int oog_table[NANGLES];
    static int initialized = 0;
    register int i;

    if (!initialized)
    { /* set up perimeter table */
        double eps[NANGLES], ua, va, ang, epsa;
        int ui, vi, ustep;
        for (i = NANGLES; i--;)
            eps[i] = 2.;
        for (vi = UV_NVS; vi--;)
        {
            va = UV_VSTART + (vi + .5) * UV_SQSIZ;
            ustep = uv_row[vi].nus - 1;
            if (vi == UV_NVS - 1 || vi == 0 || ustep <= 0)
                ustep = 1;
            for (ui = uv_row[vi].nus - 1; ui >= 0; ui -= ustep)
            {
                ua = uv_row[vi].ustart + (ui + .5) * UV_SQSIZ;
                ang = uv2ang(ua, va);
                i = (int)ang;
                epsa = fabs(ang - (i + .5));
                if (epsa < eps[i])
                {
                    oog_table[i] = uv_row[vi].ncum + ui;
                    eps[i] = epsa;
                }
            }
        }
        for (i = NANGLES; i--;) /* fill any holes */
            if (eps[i] > 1.5)
            {
                int i1, i2;
                for (i1 = 1; i1 < NANGLES / 2; i1++)
                    if (eps[(i + i1) % NANGLES] < 1.5)
                        break;
                for (i2 = 1; i2 < NANGLES / 2; i2++)
                    if (eps[(i + NANGLES - i2) % NANGLES] < 1.5)
                        break;
                if (i1 < i2)
                    oog_table[i] = oog_table[(i + i1) % NANGLES];
                else
                    oog_table[i] = oog_table[(i + NANGLES - i2) % NANGLES];
            }
        initialized = 1;
    }
    i = (int)uv2ang(u, v); /* look up hue angle */
    return (oog_table[i]);
}