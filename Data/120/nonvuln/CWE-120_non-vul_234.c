AP4_UuidAtom::AP4_UuidAtom(AP4_UI64 size, const AP4_UI08* uuid, AP4_UI08 version, AP4_UI32 flags) : 
    AP4_Atom(AP4_ATOM_TYPE_UUID, size, false, version, flags)
{
    AP4_CopyMemory(m_Uuid, uuid, 16);
}