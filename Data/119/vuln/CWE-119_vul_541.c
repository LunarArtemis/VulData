SkScalerContext_FreeType::SkScalerContext_FreeType(SkTypeface* typeface, const SkDescriptor* desc)
    : SkScalerContext_FreeType_Base(typeface, desc)
{
    SkAutoMutexAcquire  ac(gFTMutex);

    if (!ref_ft_library()) {
        sk_throw();
    }

    // load the font file
    fStrikeIndex = -1;
    fFTSize = nullptr;
    fFace = ref_ft_face(typeface);
    if (nullptr == fFace) {
        return;
    }

    fRec.computeMatrices(SkScalerContextRec::kFull_PreMatrixScale, &fScale, &fMatrix22Scalar);
    fMatrix22Scalar.setSkewX(-fMatrix22Scalar.getSkewX());
    fMatrix22Scalar.setSkewY(-fMatrix22Scalar.getSkewY());

    fScaleX = SkScalarToFDot6(fScale.fX);
    fScaleY = SkScalarToFDot6(fScale.fY);
    fMatrix22.xx = SkScalarToFixed(fMatrix22Scalar.getScaleX());
    fMatrix22.xy = SkScalarToFixed(fMatrix22Scalar.getSkewX());
    fMatrix22.yx = SkScalarToFixed(fMatrix22Scalar.getSkewY());
    fMatrix22.yy = SkScalarToFixed(fMatrix22Scalar.getScaleY());

    fLCDIsVert = SkToBool(fRec.fFlags & SkScalerContext::kLCD_Vertical_Flag);

    // compute the flags we send to Load_Glyph
    bool linearMetrics = SkToBool(fRec.fFlags & SkScalerContext::kSubpixelPositioning_Flag);
    {
        FT_Int32 loadFlags = FT_LOAD_DEFAULT;

        if (SkMask::kBW_Format == fRec.fMaskFormat) {
            // See http://code.google.com/p/chromium/issues/detail?id=43252#c24
            loadFlags = FT_LOAD_TARGET_MONO;
            if (fRec.getHinting() == SkPaint::kNo_Hinting) {
                loadFlags = FT_LOAD_NO_HINTING;
                linearMetrics = true;
            }
        } else {
            switch (fRec.getHinting()) {
            case SkPaint::kNo_Hinting:
                loadFlags = FT_LOAD_NO_HINTING;
                linearMetrics = true;
                break;
            case SkPaint::kSlight_Hinting:
                loadFlags = FT_LOAD_TARGET_LIGHT;  // This implies FORCE_AUTOHINT
                break;
            case SkPaint::kNormal_Hinting:
                if (fRec.fFlags & SkScalerContext::kForceAutohinting_Flag) {
                    loadFlags = FT_LOAD_FORCE_AUTOHINT;
#ifdef SK_BUILD_FOR_ANDROID_FRAMEWORK
                } else {
                    loadFlags = FT_LOAD_NO_AUTOHINT;
#endif
                }
                break;
            case SkPaint::kFull_Hinting:
                if (fRec.fFlags & SkScalerContext::kForceAutohinting_Flag) {
                    loadFlags = FT_LOAD_FORCE_AUTOHINT;
                    break;
                }
                loadFlags = FT_LOAD_TARGET_NORMAL;
                if (isLCD(fRec)) {
                    if (fLCDIsVert) {
                        loadFlags = FT_LOAD_TARGET_LCD_V;
                    } else {
                        loadFlags = FT_LOAD_TARGET_LCD;
                    }
                }
                break;
            default:
                SkDebugf("---------- UNKNOWN hinting %d\n", fRec.getHinting());
                break;
            }
        }

        if ((fRec.fFlags & SkScalerContext::kEmbeddedBitmapText_Flag) == 0) {
            loadFlags |= FT_LOAD_NO_BITMAP;
        }

        // Always using FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH to get correct
        // advances, as fontconfig and cairo do.
        // See http://code.google.com/p/skia/issues/detail?id=222.
        loadFlags |= FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

        // Use vertical layout if requested.
        if (fRec.fFlags & SkScalerContext::kVertical_Flag) {
            loadFlags |= FT_LOAD_VERTICAL_LAYOUT;
        }

        loadFlags |= FT_LOAD_COLOR;

        fLoadGlyphFlags = loadFlags;
    }

    FT_Error err = FT_New_Size(fFace, &fFTSize);
    if (err != 0) {
        SkDEBUGF(("FT_New_Size returned %x for face %s\n", err, fFace->family_name));
        fFace = nullptr;
        return;
    }
    err = FT_Activate_Size(fFTSize);
    if (err != 0) {
        SkDEBUGF(("FT_Activate_Size(%08x, 0x%x, 0x%x) returned 0x%x\n", fFace, fScaleX, fScaleY,
                  err));
        fFTSize = nullptr;
        return;
    }

    if (FT_IS_SCALABLE(fFace)) {
        err = FT_Set_Char_Size(fFace, fScaleX, fScaleY, 72, 72);
        if (err != 0) {
            SkDEBUGF(("FT_Set_CharSize(%08x, 0x%x, 0x%x) returned 0x%x\n",
                                    fFace, fScaleX, fScaleY,      err));
            fFace = nullptr;
            return;
        }
        FT_Set_Transform(fFace, &fMatrix22, nullptr);
    } else if (FT_HAS_FIXED_SIZES(fFace)) {
        fStrikeIndex = chooseBitmapStrike(fFace, fScaleY);
        if (fStrikeIndex == -1) {
            SkDEBUGF(("no glyphs for font \"%s\" size %f?\n",
                            fFace->family_name,       SkFDot6ToScalar(fScaleY)));
        } else {
            // FreeType does no provide linear metrics for bitmap fonts.
            linearMetrics = false;

            // FreeType documentation says:
            // FT_LOAD_NO_BITMAP -- Ignore bitmap strikes when loading.
            // Bitmap-only fonts ignore this flag.
            //
            // However, in FreeType 2.5.1 color bitmap only fonts do not ignore this flag.
            // Force this flag off for bitmap only fonts.
            fLoadGlyphFlags &= ~FT_LOAD_NO_BITMAP;
        }
    } else {
        SkDEBUGF(("unknown kind of font \"%s\" size %f?\n",
                            fFace->family_name,     SkFDot6ToScalar(fScaleY)));
    }

    fDoLinearMetrics = linearMetrics;
}