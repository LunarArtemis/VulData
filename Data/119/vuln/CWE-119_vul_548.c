int CPDF_DIBSource::CreateDecoder() {
  const CFX_ByteString& decoder = m_pStreamAcc->GetImageDecoder();
  if (decoder.IsEmpty()) {
    return 1;
  }
  if (m_bDoBpcCheck && m_bpc == 0) {
    return 0;
  }
  const uint8_t* src_data = m_pStreamAcc->GetData();
  uint32_t src_size = m_pStreamAcc->GetSize();
  const CPDF_Dictionary* pParams = m_pStreamAcc->GetImageParam();
  if (decoder == "CCITTFaxDecode") {
    m_pDecoder.reset(FPDFAPI_CreateFaxDecoder(src_data, src_size, m_Width,
                                              m_Height, pParams));
  } else if (decoder == "DCTDecode") {
    m_pDecoder.reset(CPDF_ModuleMgr::Get()->GetJpegModule()->CreateDecoder(
        src_data, src_size, m_Width, m_Height, m_nComponents,
        pParams ? pParams->GetIntegerBy("ColorTransform", 1) : 1));
    if (!m_pDecoder) {
      FX_BOOL bTransform = FALSE;
      int comps;
      int bpc;
      CCodec_JpegModule* pJpegModule = CPDF_ModuleMgr::Get()->GetJpegModule();
      if (pJpegModule->LoadInfo(src_data, src_size, m_Width, m_Height, comps,
                                bpc, bTransform)) {
        if (m_nComponents != static_cast<uint32_t>(comps)) {
          FX_Free(m_pCompData);
          m_nComponents = static_cast<uint32_t>(comps);
          if (m_Family == PDFCS_LAB && m_nComponents != 3) {
            m_pCompData = nullptr;
            return 0;
          }
          m_pCompData = GetDecodeAndMaskArray(m_bDefaultDecode, m_bColorKey);
          if (!m_pCompData) {
            return 0;
          }
        }
        m_bpc = bpc;
        m_pDecoder.reset(CPDF_ModuleMgr::Get()->GetJpegModule()->CreateDecoder(
            src_data, src_size, m_Width, m_Height, m_nComponents, bTransform));
      }
    }
  } else if (decoder == "FlateDecode") {
    m_pDecoder.reset(FPDFAPI_CreateFlateDecoder(
        src_data, src_size, m_Width, m_Height, m_nComponents, m_bpc, pParams));
  } else if (decoder == "JPXDecode") {
    LoadJpxBitmap();
    return m_pCachedBitmap ? 1 : 0;
  } else if (decoder == "JBIG2Decode") {
    m_pCachedBitmap.reset(new CFX_DIBitmap);
    if (!m_pCachedBitmap->Create(
            m_Width, m_Height, m_bImageMask ? FXDIB_1bppMask : FXDIB_1bppRgb)) {
      m_pCachedBitmap.reset();
      return 0;
    }
    m_Status = 1;
    return 2;
  } else if (decoder == "RunLengthDecode") {
    m_pDecoder.reset(CPDF_ModuleMgr::Get()
                         ->GetCodecModule()
                         ->GetBasicModule()
                         ->CreateRunLengthDecoder(src_data, src_size, m_Width,
                                                  m_Height, m_nComponents,
                                                  m_bpc));
  }
  if (!m_pDecoder)
    return 0;

  FX_SAFE_UINT32 requested_pitch =
      CalculatePitch8(m_bpc, m_nComponents, m_Width);
  if (!requested_pitch.IsValid()) {
    return 0;
  }
  FX_SAFE_UINT32 provided_pitch = CalculatePitch8(
      m_pDecoder->GetBPC(), m_pDecoder->CountComps(), m_pDecoder->GetWidth());
  if (!provided_pitch.IsValid()) {
    return 0;
  }
  if (provided_pitch.ValueOrDie() < requested_pitch.ValueOrDie()) {
    return 0;
  }
  return 1;
}