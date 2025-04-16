void CCodec_ScanlineDecoder::DownScale(int dest_width, int dest_height)
{
    if (dest_width < 0) {
        dest_width = -dest_width;
    }
    if (dest_height < 0) {
        dest_height = -dest_height;
    }
    v_DownScale(dest_width, dest_height);
    if (m_pDataCache) {
        if (m_pDataCache->m_Height == m_OutputHeight && m_pDataCache->m_Width == m_OutputWidth) {
            return;
        }
        FX_Free(m_pDataCache);
        m_pDataCache = NULL;
    }
    m_pDataCache = (CCodec_ImageDataCache*)FX_AllocNL(FX_BYTE, sizeof(CCodec_ImageDataCache) + m_Pitch * m_OutputHeight);
    if (m_pDataCache == NULL) {
        return;
    }
    m_pDataCache->m_Height = m_OutputHeight;
    m_pDataCache->m_Width = m_OutputWidth;
    m_pDataCache->m_nCachedLines = 0;
}