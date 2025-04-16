static
#endif
    int
    uv_decode(double *up, double *vp, int c) /* decode (u',v') index */
{
    int upper, lower;
    register int ui, vi;

    if (c < 0 || c >= UV_NDIVS)
        return (-1);
    lower = 0; /* binary search */
    upper = UV_NVS;
    while (upper - lower > 1)
    {
        vi = (lower + upper) >> 1;
        ui = c - uv_row[vi].ncum;
        if (ui > 0)
            lower = vi;
        else if (ui < 0)
            upper = vi;
        else
        {
            lower = vi;
            break;
        }
    }
    vi = lower;
    ui = c - uv_row[vi].ncum;
    *up = uv_row[vi].ustart + (ui + .5) * UV_SQSIZ;
    *vp = UV_VSTART + (vi + .5) * UV_SQSIZ;
    return (0);
}