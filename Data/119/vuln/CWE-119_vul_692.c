static SkColorFilter* createColorFilter(ColorMatrixType type, const float* values)
{
    SkScalar matrix[20];
    switch (type) {
    case FECOLORMATRIX_TYPE_UNKNOWN:
        break;
    case FECOLORMATRIX_TYPE_MATRIX:
        for (int i = 0; i < 20; ++i)
            matrix[i] = values[i];

        matrix[4] *= SkScalar(255);
        matrix[9] *= SkScalar(255);
        matrix[14] *= SkScalar(255);
        matrix[19] *= SkScalar(255);
        break;
    case FECOLORMATRIX_TYPE_SATURATE:
        saturateMatrix(values[0], matrix);
        break;
    case FECOLORMATRIX_TYPE_HUEROTATE:
        hueRotateMatrix(values[0], matrix);
        break;
    case FECOLORMATRIX_TYPE_LUMINANCETOALPHA:
        luminanceToAlphaMatrix(matrix);
        break;
    }
    return SkColorMatrixFilter::Create(matrix);
}