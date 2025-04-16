static
#endif
    void
    XYZtoRGB24(float *xyz, uint8_t *rgb)
{
    double r, g, b;
    /* assume CCIR-709 primaries */
    r = 2.690 * xyz[0] + -1.276 * xyz[1] + -0.414 * xyz[2];
    g = -1.022 * xyz[0] + 1.978 * xyz[1] + 0.044 * xyz[2];
    b = 0.061 * xyz[0] + -0.224 * xyz[1] + 1.163 * xyz[2];
    /* assume 2.0 gamma for speed */
    /* could use integer sqrt approx., but this is probably faster */
    rgb[0] = (uint8_t)((r <= 0.) ? 0 : (r >= 1.) ? 255 : (int)(256. * sqrt(r)));
    rgb[1] = (uint8_t)((g <= 0.) ? 0 : (g >= 1.) ? 255 : (int)(256. * sqrt(g)));
    rgb[2] = (uint8_t)((b <= 0.) ? 0 : (b >= 1.) ? 255 : (int)(256. * sqrt(b)));
}