static void luminanceToAlphaMatrix(SkScalar matrix[20])
{
    memset(matrix, 0, 20 * sizeof(SkScalar));
    matrix[15] = 0.2125f;
    matrix[16] = 0.7154f;
    matrix[17] = 0.0721f;
}