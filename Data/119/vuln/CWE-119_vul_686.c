bool FEColorMatrix::affectsTransparentPixels()
{
    // Because the input pixels are premultiplied, the only way clear pixels can be
    // painted is if the additive component for the alpha is not 0.
    return m_type == FECOLORMATRIX_TYPE_MATRIX && m_values[19] > 0;
}