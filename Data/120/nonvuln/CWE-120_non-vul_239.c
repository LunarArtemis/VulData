AP4_Size
AP4_UuidAtom::GetHeaderSize() const
{
    return (m_IsFull ? AP4_FULL_UUID_ATOM_HEADER_SIZE : AP4_UUID_ATOM_HEADER_SIZE)+(m_Size32==1?8:0);
}