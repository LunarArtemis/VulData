FX_BOOL CFX_DIBitmap::Create(int width, int height, FXDIB_Format format, FX_LPBYTE pBuffer, int pitch)
{
    m_pBuffer = NULL;
    m_bpp = (FX_BYTE)format;
    m_AlphaFlag = (FX_BYTE)(format >> 8);
    m_Width = m_Height = m_Pitch = 0;
    if (width <= 0 || height <= 0 || pitch < 0) {
        return FALSE;
    }
    if ((INT_MAX - 31) / width < (format & 0xff)) {
        return FALSE;
    }
    if (!pitch) {
        pitch = (width * (format & 0xff) + 31) / 32 * 4;
    }
    if ((1 << 30) / pitch < height) {
        return FALSE;
    }
    if (pBuffer) {
        m_pBuffer = pBuffer;
        m_bExtBuf = TRUE;
    } else {
        int size = pitch * height + 4;
        int oomlimit = _MAX_OOM_LIMIT_;
        if (oomlimit >= 0 && size >= oomlimit) {
            m_pBuffer = FX_AllocNL(FX_BYTE, size);
        } else {
            m_pBuffer = FX_Alloc(FX_BYTE, size);
        }
        if (m_pBuffer == NULL) {
            return FALSE;
        }
    }
    m_Width = width;
    m_Height = height;
    m_Pitch = pitch;
    if (HasAlpha() && format != FXDIB_Argb) {
        FX_BOOL ret = TRUE;
        ret = BuildAlphaMask();
        if (!ret) {
            if (!m_bExtBuf && m_pBuffer) {
                FX_Free(m_pBuffer);
                m_pBuffer = NULL;
                m_Width = m_Height = m_Pitch = 0;
                return FALSE;
            }
        }
    }
    return TRUE;
}