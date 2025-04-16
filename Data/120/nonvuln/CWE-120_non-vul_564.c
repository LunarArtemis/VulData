void ff_vorbis_inverse_coupling(float *mag, float *ang, int blocksize)
{
    int i;
    for (i = 0;  i < blocksize;  i++) {
        if (mag[i] > 0.0) {
            if (ang[i] > 0.0) {
                ang[i] = mag[i] - ang[i];
            } else {
                float temp = ang[i];
                ang[i]     = mag[i];
                mag[i]    += temp;
            }
        } else {
            if (ang[i] > 0.0) {
                ang[i] += mag[i];
            } else {
                float temp = ang[i];
                ang[i]     = mag[i];
                mag[i]    -= temp;
            }
        }
    }
}