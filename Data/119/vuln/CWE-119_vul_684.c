bool FEColorMatrix::setValues(const Vector<float> &values)
{
    if (m_values == values)
        return false;
    m_values = values;
    return true;
}