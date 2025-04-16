FX_BOOL CStretchEngine::StartStretchHorz()
{
    if (m_DestWidth == 0 || m_pDestScanline == NULL || m_SrcClip.Height() > (int)((1U << 29) / m_InterPitch) || m_SrcClip.Height() == 0) {
        return FALSE;
    }
    m_pInterBuf = FX_AllocNL(unsigned char, m_SrcClip.Height() * m_InterPitch);
    if (m_pInterBuf == NULL) {
        return FALSE;
    }
    if (m_pSource && m_bHasAlpha && m_pSource->m_pAlphaMask) {
        m_pExtraAlphaBuf = FX_Alloc(unsigned char, m_SrcClip.Height() * m_ExtraMaskPitch);
        if (!m_pExtraAlphaBuf) {
            return FALSE;
        }
        FX_DWORD size = (m_DestClip.Width() * 8 + 31) / 32 * 4;
        m_pDestMaskScanline = FX_AllocNL(unsigned char, size);
        if (!m_pDestMaskScanline) {
            return FALSE;
        }
    }
    m_WeightTable.Calc(m_DestWidth, m_DestClip.left, m_DestClip.right, m_SrcWidth, m_SrcClip.left, m_SrcClip.right, m_Flags);
    if (m_WeightTable.m_pWeightTables == NULL) {
        return FALSE;
    }
    m_CurRow = m_SrcClip.top;
    m_State = 1;
    return TRUE;
}