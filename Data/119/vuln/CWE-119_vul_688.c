static void hueRotateMatrix(float hue, SkScalar matrix[20])
{
    float cosHue = cosf(hue * piFloat / 180);
    float sinHue = sinf(hue * piFloat / 180);
    matrix[0] = 0.213f + cosHue * 0.787f - sinHue * 0.213f;
    matrix[1] = 0.715f - cosHue * 0.715f - sinHue * 0.715f;
    matrix[2] = 0.072f - cosHue * 0.072f + sinHue * 0.928f;
    matrix[3] = matrix[4] = 0;
    matrix[5] = 0.213f - cosHue * 0.213f + sinHue * 0.143f;
    matrix[6] = 0.715f + cosHue * 0.285f + sinHue * 0.140f;
    matrix[7] = 0.072f - cosHue * 0.072f - sinHue * 0.283f;
    matrix[8] = matrix[9] = 0;
    matrix[10] = 0.213f - cosHue * 0.213f - sinHue * 0.787f;
    matrix[11] = 0.715f - cosHue * 0.715f + sinHue * 0.715f;
    matrix[12] = 0.072f + cosHue * 0.928f + sinHue * 0.072f;
    matrix[13] = matrix[14] = 0;
    matrix[15] = matrix[16] = matrix[17] = 0;
    matrix[18] = 1;
    matrix[19] = 0;
}