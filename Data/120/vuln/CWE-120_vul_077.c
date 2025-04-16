bool
ICOInput::readimg()
{
    if (m_png) {
        // subimage is a PNG
        std::string s = PNG_pvt::read_into_buffer(m_png, m_info, m_spec, m_buf);

        //std::cerr << "[ico] PNG buffer size = " << m_buf.size () << "\n";

        if (s.length()) {
            errorf("%s", s);
            return false;
        }

        return true;
    }

    // otherwise we're dealing with a DIB
    OIIO_DASSERT(m_spec.scanline_bytes() == ((size_t)m_spec.width * 4));
    m_buf.resize(m_spec.image_bytes());

    //std::cerr << "[ico] DIB buffer size = " << m_buf.size () << "\n";

    // icons < 16bpp are colour-indexed, so load the palette
    // a palette consists of 4-byte BGR quads, with the last byte unused (reserved)
    std::vector<ico_palette_entry> palette(m_palette_size);
    if (m_bpp < 16) {  // >= 16-bit icons are unpaletted
        for (int i = 0; i < m_palette_size; i++)
            if (!fread(&palette[i], 1, sizeof(ico_palette_entry)))
                return false;
    }

    // read the colour data (the 1-bit transparency is added later on)
    // scanline length in bytes (aligned to a multiple of 32 bits)
    int slb = (m_spec.width * m_bpp + 7) / 8  // real data bytes
              + (4 - ((m_spec.width * m_bpp + 7) / 8) % 4) % 4;  // padding
    std::vector<unsigned char> scanline(slb);
    ico_palette_entry* pe;
    int k;
    for (int y = m_spec.height - 1; y >= 0; y--) {
        if (!fread(&scanline[0], 1, slb))
            return false;
        for (int x = 0; x < m_spec.width; x++) {
            k = y * m_spec.width * 4 + x * 4;
            // fill the buffer
            switch (m_bpp) {
            case 1:
                pe = &palette[(scanline[x / 8] & (1 << (7 - x % 8))) != 0];
                m_buf[k + 0] = pe->r;
                m_buf[k + 1] = pe->g;
                m_buf[k + 2] = pe->b;
                break;
            case 4:
                pe           = &palette[(scanline[x / 2] & 0xF0) >> 4];
                m_buf[k + 0] = pe->r;
                m_buf[k + 1] = pe->g;
                m_buf[k + 2] = pe->b;
                // 2 pixels per byte
                pe = &palette[scanline[x / 2] & 0x0F];
                if (x == m_spec.width - 1)
                    break;  // avoid buffer overflows
                x++;
                m_buf[k + 4] = pe->r;
                m_buf[k + 5] = pe->g;
                m_buf[k + 6] = pe->b;
                /*std::cerr << "[ico] " << y << " 2*4bit pixel: "
                          << ((int)scanline[x / 2]) << " -> "
                          << ((int)(scanline[x / 2] & 0xF0) >> 4)
                          << " & " << ((int)(scanline[x / 2]) & 0x0F)
                          << "\n";*/
                break;
            case 8:
                pe           = &palette[scanline[x]];
                m_buf[k + 0] = pe->r;
                m_buf[k + 1] = pe->g;
                m_buf[k + 2] = pe->b;
                break;
                // bpp values > 8 mean non-indexed BGR(A) images
#if 0
            // doesn't seem like ICOs can really be 16-bit, where did I even get
            // this notion from?
            case 16:
                // FIXME: find out exactly which channel gets the 1 extra
                // bit; currently I assume it's green: 5B, 6G, 5R
                // extract and shift the bits
                m_buf[k + 0] = (scanline[x * 2 + 1] & 0x1F) << 3;
                m_buf[k + 1] = ((scanline[x * 2 + 1] & 0xE0) >> 3)
                               | ((scanline[x * 2 + 0] & 0x07) << 5);
                m_buf[k + 2] = scanline[x * 2 + 0] & 0xF8;
                break;
#endif
            case 24:
                m_buf[k + 0] = scanline[x * 3 + 2];
                m_buf[k + 1] = scanline[x * 3 + 1];
                m_buf[k + 2] = scanline[x * 3 + 0];
                break;
            case 32:
                m_buf[k + 0] = scanline[x * 4 + 2];
                m_buf[k + 1] = scanline[x * 4 + 1];
                m_buf[k + 2] = scanline[x * 4 + 0];
                m_buf[k + 3] = scanline[x * 4 + 3];
                break;
            }
        }
    }

    // read the 1-bit transparency for < 32-bit icons
    if (m_bpp < 32) {
        // also aligned to a multiple of 32 bits
        slb = (m_spec.width + 7) / 8                     // real data bytes
              + (4 - ((m_spec.width + 7) / 8) % 4) % 4;  // padding
        scanline.resize(slb);
        for (int y = m_spec.height - 1; y >= 0; y--) {
            if (!fread(&scanline[0], 1, slb))
                return false;
            for (int x = 0; x < m_spec.width; x += 8) {
                for (int b = 0; b < 8; b++) {  // bit
                    k = y * m_spec.width * 4 + (x + 7 - b) * 4;
                    if (scanline[x / 8] & (1 << b))
                        m_buf[k + 3] = 0;
                    else
                        m_buf[k + 3] = 255;
                }
            }
        }
    }

    return true;
}