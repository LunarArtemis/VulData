static void saturateMatrix(float s, SkScalar matrix[20])
{
    matrix[0] = 0.213f + 0.787f * s;
    matrix[1] = 0.715f - 0.715f * s;
    matrix[2] = 0.072f - 0.072f * s;
    matrix[3] = matrix[4] = 0;
    matrix[5] = 0.213f - 0.213f * s;
    matrix[6] = 0.715f + 0.285f * s;
    matrix[7] = 0.072f - 0.072f * s;
    matrix[8] = matrix[9] = 0;
    matrix[10] = 0.213f - 0.213f * s;
    matrix[11] = 0.715f - 0.715f * s;
    matrix[12] = 0.072f + 0.928f * s;
    matrix[13] = matrix[14] = 0;
    matrix[15] = matrix[16] = matrix[17] = 0;
    matrix[18] = 1;
    matrix[19] = 0;
}