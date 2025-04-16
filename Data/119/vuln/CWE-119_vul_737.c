FX_BOOL CFX_DIBitmap::ConvertFormat(FXDIB_Format dest_format, void* pIccTransform)
{
    FXDIB_Format src_format = GetFormat();
    if (dest_format == src_format && pIccTransform == NULL) {
        return TRUE;
    }
    if (dest_format == FXDIB_8bppMask && src_format == FXDIB_8bppRgb && m_pPalette == NULL) {
        m_AlphaFlag = 1;
        return TRUE;
    }
    if (dest_format == FXDIB_Argb && src_format == FXDIB_Rgb32 && pIccTransform == NULL) {
        m_AlphaFlag = 2;
        for (int row = 0; row < m_Height; row ++) {
            FX_LPBYTE scanline = m_pBuffer + row * m_Pitch + 3;
            for (int col = 0; col < m_Width; col ++) {
                *scanline = 0xff;
                scanline += 4;
            }
        }
        return TRUE;
    }
    int dest_bpp = dest_format & 0xff;
    int dest_pitch = (dest_bpp * m_Width + 31) / 32 * 4;
    FX_LPBYTE dest_buf = FX_AllocNL(FX_BYTE, dest_pitch * m_Height + 4);
    if (dest_buf == NULL) {
        return FALSE;
    }
    CFX_DIBitmap* pAlphaMask = NULL;
    if (dest_format == FXDIB_Argb) {
        FXSYS_memset8(dest_buf, 0xff, dest_pitch * m_Height + 4);
        if (m_pAlphaMask) {
            for (int row = 0; row < m_Height; row ++) {
                FX_LPBYTE pDstScanline = dest_buf + row * dest_pitch + 3;
                FX_LPCBYTE pSrcScanline = m_pAlphaMask->GetScanline(row);
                for (int col = 0; col < m_Width; col ++) {
                    *pDstScanline = *pSrcScanline++;
                    pDstScanline += 4;
                }
            }
        }
    } else if (dest_format & 0x0200) {
        if (src_format == FXDIB_Argb) {
            pAlphaMask = GetAlphaMask();
            if (pAlphaMask == NULL) {
                FX_Free(dest_buf);
                return FALSE;
            }
        } else {
            if (m_pAlphaMask == NULL) {
                if (!BuildAlphaMask()) {
                    FX_Free(dest_buf);
                    return FALSE;
                }
                pAlphaMask = m_pAlphaMask;
                m_pAlphaMask = NULL;
            } else {
                pAlphaMask = m_pAlphaMask;
            }
        }
    }
    FX_BOOL ret = FALSE;
    FX_DWORD* pal_8bpp = NULL;
    ret = ConvertBuffer(dest_format, dest_buf, dest_pitch, m_Width, m_Height, this, 0, 0, pal_8bpp, pIccTransform);
    if (!ret) {
        if (pal_8bpp) {
            FX_Free(pal_8bpp);
        }
        if (pAlphaMask != m_pAlphaMask) {
            delete pAlphaMask;
        }
        if (dest_buf) {
            FX_Free(dest_buf);
        }
        return FALSE;
    }
    if (m_pAlphaMask && pAlphaMask != m_pAlphaMask) {
        delete m_pAlphaMask;
    }
    m_pAlphaMask = pAlphaMask;
    if (m_pPalette) {
        FX_Free(m_pPalette);
    }
    m_pPalette = pal_8bpp;
    if (!m_bExtBuf) {
        FX_Free(m_pBuffer);
    }
    m_bExtBuf = FALSE;
    m_pBuffer = dest_buf;
    m_bpp = (FX_BYTE)dest_format;
    m_AlphaFlag = (FX_BYTE)(dest_format >> 8);
    m_Pitch = dest_pitch;
    return TRUE;
}