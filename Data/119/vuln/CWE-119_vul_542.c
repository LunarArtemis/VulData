void SkTypeface_FreeType::onFilterRec(SkScalerContextRec* rec) const {
    //BOGUS: http://code.google.com/p/chromium/issues/detail?id=121119
    //Cap the requested size as larger sizes give bogus values.
    //Remove when http://code.google.com/p/skia/issues/detail?id=554 is fixed.
    if (rec->fTextSize > SkIntToScalar(1 << 14)) {
        rec->fTextSize = SkIntToScalar(1 << 14);
    }

    if (isLCD(*rec)) {
        // TODO: re-work so that FreeType is set-up and selected by the SkFontMgr.
        SkAutoMutexAcquire ama(gFTMutex);
        ref_ft_library();
        if (!gFTLibrary->isLCDSupported()) {
            // If the runtime Freetype library doesn't support LCD, disable it here.
            rec->fMaskFormat = SkMask::kA8_Format;
        }
        unref_ft_library();
    }

    SkPaint::Hinting h = rec->getHinting();
    if (SkPaint::kFull_Hinting == h && !isLCD(*rec)) {
        // collapse full->normal hinting if we're not doing LCD
        h = SkPaint::kNormal_Hinting;
    }
    if ((rec->fFlags & SkScalerContext::kSubpixelPositioning_Flag)) {
        if (SkPaint::kNo_Hinting != h) {
            h = SkPaint::kSlight_Hinting;
        }
    }

    // rotated text looks bad with hinting, so we disable it as needed
    if (!isAxisAligned(*rec)) {
        h = SkPaint::kNo_Hinting;
    }
    rec->setHinting(h);

#ifndef SK_GAMMA_APPLY_TO_A8
    if (!isLCD(*rec)) {
      rec->ignorePreBlend();
    }
#endif
}