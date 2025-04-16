bool FEColorMatrix::setValues(const Vector<float> &values)
{
    ASSERT(values.size() == 20);
    if (m_values == values)
        return false;
    m_values = values;
    return true;
}