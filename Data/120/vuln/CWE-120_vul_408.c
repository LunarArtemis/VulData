inline int vc1_unescape_buffer(uint8_t* src, int size)
    {
        if (m_nalBuffer != 0)
            delete m_nalBuffer;
        m_nalBuffer = new uint8_t[size];
        int dsize = 0, i;
        if (size < 4)
        {
            for (dsize = 0; dsize < size; dsize++) *m_nalBuffer++ = *src++;
            m_nalBufferLen = size;
            return size;
        }
        for (i = 0; i < size; i++, src++)
        {
            if (src[0] == 3 && i >= 2 && !src[-1] && !src[-2] && i < size - 1 && src[1] < 4)
            {
                m_nalBuffer[dsize++] = src[1];
                src++;
                i++;
            }
            else
                m_nalBuffer[dsize++] = *src;
        }
        m_nalBufferLen = dsize;
        return dsize;
    }