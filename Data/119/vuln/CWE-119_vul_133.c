AP4_StssAtom::AP4_StssAtom(AP4_UI32        size, 
                           AP4_UI08        version,
                           AP4_UI32        flags,
                           AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_STSS, size, version, flags),
    m_LookupCache(0)
{
    AP4_UI32 entry_count;
    stream.ReadUI32(entry_count);
    
    // check for bogus values
    if (entry_count*4 > size) return;
    
    // read the table into a local array for conversion
    unsigned char* buffer = new unsigned char[entry_count*4];
    AP4_Result result = stream.Read(buffer, entry_count*4);
    if (AP4_FAILED(result)) {
        delete[] buffer;
        return;
    }
    m_Entries.SetItemCount(entry_count);
    for (unsigned int i=0; i<entry_count; i++) {
        m_Entries[i] = AP4_BytesToUInt32BE(&buffer[i*4]);
    }
    delete[] buffer;
}