bool
ICOInput::read_native_scanline(int subimage, int miplevel, int y, int /*z*/,
                               void* data)
{
    lock_guard lock(*this);
    if (!seek_subimage(subimage, miplevel))
        return false;

    if (m_buf.empty()) {
        if (!readimg())
            return false;
    }

    size_t size = spec().scanline_bytes();
    //std::cerr << "[ico] reading scanline " << y << " (" << size << " bytes)\n";
    memcpy(data, &m_buf[y * size], size);
    return true;
}