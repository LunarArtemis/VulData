CFX_GlyphBitmap* CFX_FaceCache::RenderGlyph(CFX_Font* pFont,
                                            uint32_t glyph_index,
                                            FX_BOOL bFontStyle,
                                            const CFX_Matrix* pMatrix,
                                            int dest_width,
                                            int anti_alias) {
  if (!m_Face) {
    return NULL;
  }
  FXFT_Matrix ft_matrix;
  ft_matrix.xx = (signed long)(pMatrix->GetA() / 64 * 65536);
  ft_matrix.xy = (signed long)(pMatrix->GetC() / 64 * 65536);
  ft_matrix.yx = (signed long)(pMatrix->GetB() / 64 * 65536);
  ft_matrix.yy = (signed long)(pMatrix->GetD() / 64 * 65536);
  FX_BOOL bUseCJKSubFont = FALSE;
  const CFX_SubstFont* pSubstFont = pFont->GetSubstFont();
  if (pSubstFont) {
    bUseCJKSubFont = pSubstFont->m_bSubstOfCJK && bFontStyle;
    int skew = 0;
    if (bUseCJKSubFont) {
      skew = pSubstFont->m_bItlicCJK ? -15 : 0;
    } else {
      skew = pSubstFont->m_ItalicAngle;
    }
    if (skew) {
      // skew is nonpositive so -skew is used as the index.
      skew = -skew <= static_cast<int>(ANGLESKEW_ARRAY_SIZE)
                 ? -58
                 : -g_AngleSkew[-skew];
      if (pFont->IsVertical())
        ft_matrix.yx += ft_matrix.yy * skew / 100;
      else
        ft_matrix.xy += -ft_matrix.xx * skew / 100;
    }
    if (pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) {
      pFont->AdjustMMParams(glyph_index, dest_width,
                            pFont->GetSubstFont()->m_Weight);
    }
  }
  ScopedFontTransform scoped_transform(m_Face, &ft_matrix);
  int load_flags = (m_Face->face_flags & FT_FACE_FLAG_SFNT)
                       ? FXFT_LOAD_NO_BITMAP
                       : (FXFT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
  int error = FXFT_Load_Glyph(m_Face, glyph_index, load_flags);
  if (error) {
    // if an error is returned, try to reload glyphs without hinting.
    if (load_flags & FT_LOAD_NO_HINTING || load_flags & FT_LOAD_NO_SCALE) {
      return NULL;
    }

    load_flags |= FT_LOAD_NO_HINTING;
    error = FXFT_Load_Glyph(m_Face, glyph_index, load_flags);

    if (error) {
      return NULL;
    }
  }
  int weight = 0;
  if (bUseCJKSubFont) {
    weight = pSubstFont->m_WeightCJK;
  } else {
    weight = pSubstFont ? pSubstFont->m_Weight : 0;
  }
  if (pSubstFont && !(pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) &&
      weight > 400) {
    uint32_t index = (weight - 400) / 10;
    if (index >= WEIGHTPOW_ARRAY_SIZE)
      return NULL;
    int level = 0;
    if (pSubstFont->m_Charset == FXFONT_SHIFTJIS_CHARSET) {
      level =
          g_WeightPow_SHIFTJIS[index] * 2 *
          (FXSYS_abs((int)(ft_matrix.xx)) + FXSYS_abs((int)(ft_matrix.xy))) /
          36655;
    } else {
      level = g_WeightPow_11[index] * (FXSYS_abs((int)(ft_matrix.xx)) +
                                       FXSYS_abs((int)(ft_matrix.xy))) /
              36655;
    }
    FXFT_Outline_Embolden(FXFT_Get_Glyph_Outline(m_Face), level);
  }
  FXFT_Library_SetLcdFilter(CFX_GEModule::Get()->GetFontMgr()->GetFTLibrary(),
                            FT_LCD_FILTER_DEFAULT);
  error = FXFT_Render_Glyph(m_Face, anti_alias);
  if (error) {
    return NULL;
  }
  int bmwidth = FXFT_Get_Bitmap_Width(FXFT_Get_Glyph_Bitmap(m_Face));
  int bmheight = FXFT_Get_Bitmap_Rows(FXFT_Get_Glyph_Bitmap(m_Face));
  if (bmwidth > 2048 || bmheight > 2048) {
    return NULL;
  }
  int dib_width = bmwidth;
  CFX_GlyphBitmap* pGlyphBitmap = new CFX_GlyphBitmap;
  pGlyphBitmap->m_Bitmap.Create(
      dib_width, bmheight,
      anti_alias == FXFT_RENDER_MODE_MONO ? FXDIB_1bppMask : FXDIB_8bppMask);
  pGlyphBitmap->m_Left = FXFT_Get_Glyph_BitmapLeft(m_Face);
  pGlyphBitmap->m_Top = FXFT_Get_Glyph_BitmapTop(m_Face);
  int dest_pitch = pGlyphBitmap->m_Bitmap.GetPitch();
  int src_pitch = FXFT_Get_Bitmap_Pitch(FXFT_Get_Glyph_Bitmap(m_Face));
  uint8_t* pDestBuf = pGlyphBitmap->m_Bitmap.GetBuffer();
  uint8_t* pSrcBuf =
      (uint8_t*)FXFT_Get_Bitmap_Buffer(FXFT_Get_Glyph_Bitmap(m_Face));
  if (anti_alias != FXFT_RENDER_MODE_MONO &&
      FXFT_Get_Bitmap_PixelMode(FXFT_Get_Glyph_Bitmap(m_Face)) ==
          FXFT_PIXEL_MODE_MONO) {
    int bytes = anti_alias == FXFT_RENDER_MODE_LCD ? 3 : 1;
    for (int i = 0; i < bmheight; i++)
      for (int n = 0; n < bmwidth; n++) {
        uint8_t data =
            (pSrcBuf[i * src_pitch + n / 8] & (0x80 >> (n % 8))) ? 255 : 0;
        for (int b = 0; b < bytes; b++) {
          pDestBuf[i * dest_pitch + n * bytes + b] = data;
        }
      }
  } else {
    FXSYS_memset(pDestBuf, 0, dest_pitch * bmheight);
    if (anti_alias == FXFT_RENDER_MODE_MONO &&
        FXFT_Get_Bitmap_PixelMode(FXFT_Get_Glyph_Bitmap(m_Face)) ==
            FXFT_PIXEL_MODE_MONO) {
      int rowbytes =
          FXSYS_abs(src_pitch) > dest_pitch ? dest_pitch : FXSYS_abs(src_pitch);
      for (int row = 0; row < bmheight; row++) {
        FXSYS_memcpy(pDestBuf + row * dest_pitch, pSrcBuf + row * src_pitch,
                     rowbytes);
      }
    } else {
      _ContrastAdjust(pSrcBuf, pDestBuf, bmwidth, bmheight, src_pitch,
                      dest_pitch);
      _GammaAdjust(pDestBuf, bmwidth, bmheight, dest_pitch,
                   CFX_GEModule::Get()->GetTextGammaTable());
    }
  }
  return pGlyphBitmap;
}