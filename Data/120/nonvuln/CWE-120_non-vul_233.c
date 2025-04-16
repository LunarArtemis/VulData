AP4_UuidAtom::AP4_UuidAtom(AP4_UI64 size, bool is_full, AP4_ByteStream& stream) : 
    AP4_Atom(AP4_ATOM_TYPE_UUID, size)
{
    if (is_full) {
        m_IsFull = true;
        ReadFullHeader(stream, m_Version, m_Flags);
    }
}