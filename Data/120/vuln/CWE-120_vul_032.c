AP4_UnknownUuidAtom::AP4_UnknownUuidAtom(AP4_UI64 size, const AP4_UI08* uuid, AP4_ByteStream& stream) : 
    AP4_UuidAtom(size, uuid)
{
    // store the data
    m_Data.SetDataSize((AP4_Size)size-GetHeaderSize());
    stream.Read(m_Data.UseData(), m_Data.GetDataSize());
}