AP4_Result
AP4_UuidAtom::WriteHeader(AP4_ByteStream& stream)
{
    AP4_Result result;

    // write the size
    result = stream.WriteUI32(m_Size32);
    if (AP4_FAILED(result)) return result;

    // write the type
    result = stream.WriteUI32(m_Type);
    if (AP4_FAILED(result)) return result;

    // handle 64-bit sizes
    if (m_Size32 == 1) {
        result = stream.WriteUI64(m_Size64);
        if (AP4_FAILED(result)) return result;
    }

    // write the extended type 
    result = stream.Write(m_Uuid, 16);
    if (AP4_FAILED(result)) return result;
    
    // for full atoms, write version and flags
    if (m_IsFull) {
        result = stream.WriteUI08((AP4_UI08)m_Version);
        if (AP4_FAILED(result)) return result;
        result = stream.WriteUI24(m_Flags);
        if (AP4_FAILED(result)) return result;
    }

    return AP4_SUCCESS;
}