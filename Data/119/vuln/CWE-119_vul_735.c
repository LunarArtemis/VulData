CStretchEngine::CStretchEngine(IFX_ScanlineComposer* pDestBitmap, FXDIB_Format dest_format,
                               int dest_width, int dest_height, const FX_RECT& clip_rect,
                               const CFX_DIBSource* pSrcBitmap, int flags)
{
    m_State = 0;
    m_DestFormat = dest_format;
    m_DestBpp = dest_format & 0xff;
    m_SrcBpp = pSrcBitmap->GetFormat() & 0xff;
    m_bHasAlpha = pSrcBitmap->GetFormat() & 0x200;
    m_pSrcPalette = pSrcBitmap->GetPalette();
    m_pDestBitmap = pDestBitmap;
    m_DestWidth = dest_width;
    m_DestHeight = dest_height;
    m_pInterBuf = NULL;
    m_pExtraAlphaBuf = NULL;
    m_pDestMaskScanline = NULL;
    m_DestClip = clip_rect;
    FX_DWORD size = clip_rect.Width();
    if (size && m_DestBpp > (int)(INT_MAX / size)) {
        return;
    }
    size *= m_DestBpp;
    if (size > INT_MAX - 31) {
        return;
    }
    size += 31;
    size = size / 32 * 4;
    m_pDestScanline = FX_AllocNL(FX_BYTE, size);
    if (m_pDestScanline == NULL) {
        return;
    }
    if (dest_format == FXDIB_Rgb32) {
        FXSYS_memset8(m_pDestScanline, 255, size);
    }
    m_InterPitch = (m_DestClip.Width() * m_DestBpp + 31) / 32 * 4;
    m_ExtraMaskPitch = (m_DestClip.Width() * 8 + 31) / 32 * 4;
    m_pInterBuf = NULL;
    m_pSource = pSrcBitmap;
    m_SrcWidth = pSrcBitmap->GetWidth();
    m_SrcHeight = pSrcBitmap->GetHeight();
    m_SrcPitch = (m_SrcWidth * m_SrcBpp + 31) / 32 * 4;
    if ((flags & FXDIB_NOSMOOTH) == 0) {
        FX_BOOL bInterpol = flags & FXDIB_INTERPOL || flags & FXDIB_BICUBIC_INTERPOL;
        if (!bInterpol && FXSYS_abs(dest_width) != 0 && FXSYS_abs(dest_height) < m_SrcWidth * m_SrcHeight * 8 / FXSYS_abs(dest_width)) {
            flags = FXDIB_INTERPOL;
        }
        m_Flags = flags;
    } else {
        m_Flags = FXDIB_NOSMOOTH;
        if (flags & FXDIB_DOWNSAMPLE) {
            m_Flags |= FXDIB_DOWNSAMPLE;
        }
    }
    double scale_x = FXSYS_Div((FX_FLOAT)(m_SrcWidth), (FX_FLOAT)(m_DestWidth));
    double scale_y = FXSYS_Div((FX_FLOAT)(m_SrcHeight), (FX_FLOAT)(m_DestHeight));
    double base_x = m_DestWidth > 0 ? 0.0f : (FX_FLOAT)(m_DestWidth);
    double base_y = m_DestHeight > 0 ? 0.0f : (FX_FLOAT)(m_DestHeight);
    double src_left = FXSYS_Mul(scale_x, (FX_FLOAT)(clip_rect.left) + base_x);
    double src_right = FXSYS_Mul(scale_x, (FX_FLOAT)(clip_rect.right) + base_x);
    double src_top = FXSYS_Mul(scale_y, (FX_FLOAT)(clip_rect.top) + base_y);
    double src_bottom = FXSYS_Mul(scale_y, (FX_FLOAT)(clip_rect.bottom) + base_y);
    if (src_left > src_right) {
        double temp = src_left;
        src_left = src_right;
        src_right = temp;
    }
    if (src_top > src_bottom) {
        double temp = src_top;
        src_top = src_bottom;
        src_bottom = temp;
    }
    m_SrcClip.left = (int)FXSYS_floor((FX_FLOAT)src_left);
    m_SrcClip.right = (int)FXSYS_ceil((FX_FLOAT)src_right);
    m_SrcClip.top = (int)FXSYS_floor((FX_FLOAT)src_top);
    m_SrcClip.bottom = (int)FXSYS_ceil((FX_FLOAT)src_bottom);
    FX_RECT src_rect(0, 0, m_SrcWidth, m_SrcHeight);
    m_SrcClip.Intersect(src_rect);
    if (m_SrcBpp == 1) {
        if (m_DestBpp == 8) {
            m_TransMethod = 1;
        } else {
            m_TransMethod = 2;
        }
    } else if (m_SrcBpp == 8) {
        if (m_DestBpp == 8) {
            if (!m_bHasAlpha) {
                m_TransMethod = 3;
            } else {
                m_TransMethod = 4;
            }
        } else {
            if (!m_bHasAlpha) {
                m_TransMethod = 5;
            } else {
                m_TransMethod = 6;
            }
        }
    } else {
        if (!m_bHasAlpha) {
            m_TransMethod = 7;
        } else {
            m_TransMethod = 8;
        }
    }
}