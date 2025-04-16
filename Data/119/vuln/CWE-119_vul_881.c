bool ATSParser::PSISection::isCRCOkay() const {
    if (!isComplete()) {
        return false;
    }
    uint8_t* data = mBuffer->data();

    // Return true if section_syntax_indicator says no section follows the field section_length.
    if ((data[1] & 0x80) == 0) {
        return true;
    }

    unsigned sectionLength = U16_AT(data + 1) & 0xfff;
    ALOGV("sectionLength %u, skip %u", sectionLength, mSkipBytes);

    // Skip the preceding field present when payload start indicator is on.
    sectionLength -= mSkipBytes;

    uint32_t crc = 0xffffffff;
    for(unsigned i = 0; i < sectionLength + 4 /* crc */; i++) {
        uint8_t b = data[i];
        int index = ((crc >> 24) ^ (b & 0xff)) & 0xff;
        crc = CRC_TABLE[index] ^ (crc << 8);
    }
    ALOGV("crc: %08x\n", crc);
    return (crc == 0);
}