CFX_PathData* CFX_Font::LoadGlyphPath(uint32_t glyph_index, int dest_width) {
  if (!m_Face) {
    return NULL;
  }
  FXFT_Set_Pixel_Sizes(m_Face, 0, 64);
  FXFT_Matrix ft_matrix = {65536, 0, 0, 65536};
  if (m_pSubstFont) {
    if (m_pSubstFont->m_ItalicAngle) {
      int skew = m_pSubstFont->m_ItalicAngle;
      // skew is nonpositive so -skew is used as the index.
      skew = -skew <= static_cast<int>(ANGLESKEW_ARRAY_SIZE)
                 ? -58
                 : -g_AngleSkew[-skew];
      if (m_bVertical)
        ft_matrix.yx += ft_matrix.yy * skew / 100;
      else
        ft_matrix.xy += -ft_matrix.xx * skew / 100;
    }
    if (m_pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) {
      AdjustMMParams(glyph_index, dest_width, m_pSubstFont->m_Weight);
    }
  }
  ScopedFontTransform scoped_transform(m_Face, &ft_matrix);
  int load_flags = FXFT_LOAD_NO_BITMAP;
  if (!(m_Face->face_flags & FT_FACE_FLAG_SFNT) || !FT_IS_TRICKY(m_Face)) {
    load_flags |= FT_LOAD_NO_HINTING;
  }
  if (FXFT_Load_Glyph(m_Face, glyph_index, load_flags))
    return NULL;
  if (m_pSubstFont && !(m_pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) &&
      m_pSubstFont->m_Weight > 400) {
    uint32_t index = (m_pSubstFont->m_Weight - 400) / 10;
    if (index >= WEIGHTPOW_ARRAY_SIZE)
      index = WEIGHTPOW_ARRAY_SIZE - 1;
    int level = 0;
    if (m_pSubstFont->m_Charset == FXFONT_SHIFTJIS_CHARSET)
      level = g_WeightPow_SHIFTJIS[index] * 2 * 65536 / 36655;
    else
      level = g_WeightPow[index] * 2;
    FXFT_Outline_Embolden(FXFT_Get_Glyph_Outline(m_Face), level);
  }
  FXFT_Outline_Funcs funcs;
  funcs.move_to = _Outline_MoveTo;
  funcs.line_to = _Outline_LineTo;
  funcs.conic_to = _Outline_ConicTo;
  funcs.cubic_to = _Outline_CubicTo;
  funcs.shift = 0;
  funcs.delta = 0;
  OUTLINE_PARAMS params;
  params.m_bCount = TRUE;
  params.m_PointCount = 0;
  FXFT_Outline_Decompose(FXFT_Get_Glyph_Outline(m_Face), &funcs, &params);
  if (params.m_PointCount == 0) {
    return NULL;
  }
  CFX_PathData* pPath = new CFX_PathData;
  pPath->SetPointCount(params.m_PointCount);
  params.m_bCount = FALSE;
  params.m_PointCount = 0;
  params.m_pPoints = pPath->GetPoints();
  params.m_CurX = params.m_CurY = 0;
  params.m_CoordUnit = 64 * 64.0;
  FXFT_Outline_Decompose(FXFT_Get_Glyph_Outline(m_Face), &funcs, &params);
  _Outline_CheckEmptyContour(&params);
  pPath->TrimPoints(params.m_PointCount);
  if (params.m_PointCount) {
    pPath->GetPoints()[params.m_PointCount - 1].m_Flag |= FXPT_CLOSEFIGURE;
  }
  return pPath;
}