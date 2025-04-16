AP4_UnknownUuidAtom::AP4_UnknownUuidAtom(AP4_UI64 size, AP4_ByteStream& stream) : 
    AP4_UuidAtom(size, false, stream)
{
    // store the data
    m_Data.SetDataSize((AP4_Size)size-GetHeaderSize());
    stream.Read(m_Data.UseData(), m_Data.GetDataSize());
}