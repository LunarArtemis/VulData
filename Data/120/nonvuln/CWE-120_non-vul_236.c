AP4_Result
AP4_UnknownUuidAtom::WriteFields(AP4_ByteStream& stream)
{
    // write the data
    return stream.Write(m_Data.GetData(), m_Data.GetDataSize());
}