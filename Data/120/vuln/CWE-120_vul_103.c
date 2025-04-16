AP4_Result
AP4_HdlrAtom::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;

    // write the data
    result = stream.WriteUI32(m_Predefined);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI32(m_HandlerType);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI32(m_Reserved[0]);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI32(m_Reserved[1]);
    if (AP4_FAILED(result)) return result;
    result = stream.WriteUI32(m_Reserved[2]);
    if (AP4_FAILED(result)) return result;
    AP4_UI08 name_size = (AP4_UI08)m_HandlerName.GetLength();
    if (m_QuickTimeMode) {
        name_size += 1;
        if (AP4_FULL_ATOM_HEADER_SIZE + 20 + name_size > m_Size32) {
            name_size = (AP4_UI08)(m_Size32 - (AP4_FULL_ATOM_HEADER_SIZE + 20));
        }
        if (name_size) {
            result = stream.WriteUI08(name_size - 1);
            if (AP4_FAILED(result)) return result;

            result = stream.Write(m_HandlerName.GetChars(), name_size - 1);
            if (AP4_FAILED(result)) return result;
        }
    } else {
        if (AP4_FULL_ATOM_HEADER_SIZE + 20 + name_size > m_Size32) {
            name_size = (AP4_UI08)(m_Size32 - (AP4_FULL_ATOM_HEADER_SIZE + 20));
        }
        if (name_size) {
            result = stream.Write(m_HandlerName.GetChars(), name_size);
            if (AP4_FAILED(result)) return result;
        }
    }

    // pad with zeros if necessary
    if (m_Size32 > AP4_FULL_ATOM_HEADER_SIZE + 20 + name_size) {
        AP4_Size padding = m_Size32 - (AP4_FULL_ATOM_HEADER_SIZE + 20 + name_size);
        while (padding--) stream.WriteUI08(0);
    }

    return AP4_SUCCESS;
}