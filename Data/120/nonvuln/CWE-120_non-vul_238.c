AP4_Result
AP4_UuidAtom::InspectHeader(AP4_AtomInspector& inspector)
{
    char uuid[37];
    uuid[36] = '\0';
    char* dst = uuid;
    for (unsigned int i=0; i<16; i++) {
        *dst++ = AP4_NibbleHex(m_Uuid[i]>>4);
        *dst++ = AP4_NibbleHex(m_Uuid[i]&0x0F);
        if (i == 5 || i == 7 || i == 9 || i == 11) *dst++ = '-';
    }
    
    inspector.StartAtom(uuid, 
                        m_Version,
                        m_Flags,
                        GetHeaderSize(),
                        GetSize());

    return AP4_SUCCESS;
}