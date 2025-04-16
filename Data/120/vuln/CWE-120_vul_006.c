void CiffDirectory::doRead(const byte* pData,
                               uint32_t    size,
                               uint32_t    start,
                               ByteOrder   byteOrder)
    {
        CiffComponent::doRead(pData, size, start, byteOrder);
#ifdef EXIV2_DEBUG_MESSAGES
        std::cout << "Reading directory 0x" << std::hex << tag() << "\n";
#endif
        readDirectory(pData + offset(), this->size(), byteOrder);
#ifdef EXIV2_DEBUG_MESSAGES
        std::cout << "<---- 0x" << std::hex << tag() << "\n";
#endif
    }