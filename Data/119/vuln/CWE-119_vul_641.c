void GrDistanceFieldTextContext::appendGlyph(GrGlyph::PackedID packed,
                                             SkFixed vx, SkFixed vy,
                                             GrFontScaler* scaler) {
    if (NULL == fDrawTarget) {
        return;
    }

    if (NULL == fStrike) {
        fStrike = fContext->getFontCache()->getStrike(scaler, true);
    }

    GrGlyph* glyph = fStrike->getGlyph(packed, scaler);
    if (NULL == glyph || glyph->fBounds.isEmpty()) {
        return;
    }

    // TODO: support color glyphs
    if (kA8_GrMaskFormat != glyph->fMaskFormat) {
        return;
    }

    SkScalar sx = SkFixedToScalar(vx);
    SkScalar sy = SkFixedToScalar(vy);
/*
    // not valid, need to find a different solution for this
    vx += SkIntToFixed(glyph->fBounds.fLeft);
    vy += SkIntToFixed(glyph->fBounds.fTop);

    // keep them as ints until we've done the clip-test
    GrFixed width = glyph->fBounds.width();
    GrFixed height = glyph->fBounds.height();

    // check if we clipped out
    if (true || NULL == glyph->fPlot) {
        int x = vx >> 16;
        int y = vy >> 16;
        if (fClipRect.quickReject(x, y, x + width, y + height)) {
//            SkCLZ(3);    // so we can set a break-point in the debugger
            return;
        }
    }
*/
    if (NULL == glyph->fPlot) {
        if (!fStrike->glyphTooLargeForAtlas(glyph)) {
            if (fStrike->addGlyphToAtlas(glyph, scaler)) {
                goto HAS_ATLAS;
            }

            // try to clear out an unused plot before we flush
            if (fContext->getFontCache()->freeUnusedPlot(fStrike, glyph) &&
                fStrike->addGlyphToAtlas(glyph, scaler)) {
                goto HAS_ATLAS;
            }

            if (c_DumpFontCache) {
#ifdef SK_DEVELOPER
                fContext->getFontCache()->dump();
#endif
            }

            // before we purge the cache, we must flush any accumulated draws
            this->flush();
            fContext->flush();

            // we should have an unused plot now
            if (fContext->getFontCache()->freeUnusedPlot(fStrike, glyph) &&
                fStrike->addGlyphToAtlas(glyph, scaler)) {
                goto HAS_ATLAS;
            }
        }

        if (NULL == glyph->fPath) {
            SkPath* path = SkNEW(SkPath);
            if (!scaler->getGlyphPath(glyph->glyphID(), path)) {
                // flag the glyph as being dead?
                delete path;
                return;
            }
            glyph->fPath = path;
        }

        GrContext::AutoMatrix am;
        SkMatrix ctm;
        ctm.setScale(fTextRatio, fTextRatio);
        ctm.postTranslate(sx, sy);
        GrPaint tmpPaint(fPaint);
        am.setPreConcat(fContext, ctm, &tmpPaint);
        GrStrokeInfo strokeInfo(SkStrokeRec::kFill_InitStyle);
        fContext->drawPath(tmpPaint, *glyph->fPath, strokeInfo);
        return;
    }

HAS_ATLAS:
    SkASSERT(glyph->fPlot);
    GrDrawTarget::DrawToken drawToken = fDrawTarget->getCurrentDrawToken();
    glyph->fPlot->setDrawToken(drawToken);

    GrTexture* texture = glyph->fPlot->texture();
    SkASSERT(texture);

    if (fCurrTexture != texture || fCurrVertex + 4 > fMaxVertices) {
        this->flush();
        fCurrTexture = texture;
        fCurrTexture->ref();
    }

    bool useColorVerts = !fUseLCDText;

    if (NULL == fVertices) {
        // If we need to reserve vertices allow the draw target to suggest
        // a number of verts to reserve and whether to perform a flush.
        fMaxVertices = kMinRequestedVerts;
        if (useColorVerts) {
            fDrawTarget->drawState()->setVertexAttribs<gTextVertexWithColorAttribs>(
                SK_ARRAY_COUNT(gTextVertexWithColorAttribs), kTextVAColorSize);
        } else {
            fDrawTarget->drawState()->setVertexAttribs<gTextVertexAttribs>(
                SK_ARRAY_COUNT(gTextVertexAttribs), kTextVASize);
        }
        bool flush = fDrawTarget->geometryHints(&fMaxVertices, NULL);
        if (flush) {
            this->flush();
            fContext->flush();
            if (useColorVerts) {
                fDrawTarget->drawState()->setVertexAttribs<gTextVertexWithColorAttribs>(
                    SK_ARRAY_COUNT(gTextVertexWithColorAttribs), kTextVAColorSize);
            } else {
                fDrawTarget->drawState()->setVertexAttribs<gTextVertexAttribs>(
                    SK_ARRAY_COUNT(gTextVertexAttribs), kTextVASize);
            }
        }
        fMaxVertices = kDefaultRequestedVerts;
        // ignore return, no point in flushing again.
        fDrawTarget->geometryHints(&fMaxVertices, NULL);
        
        int maxQuadVertices = 4 * fContext->getQuadIndexBuffer()->maxQuads();
        if (fMaxVertices < kMinRequestedVerts) {
            fMaxVertices = kDefaultRequestedVerts;
        } else if (fMaxVertices > maxQuadVertices) {
            // don't exceed the limit of the index buffer
            fMaxVertices = maxQuadVertices;
        }
        bool success = fDrawTarget->reserveVertexAndIndexSpace(fMaxVertices,
                                                               0,
                                                               &fVertices,
                                                               NULL);
        GrAlwaysAssert(success);
    }

    SkScalar dx = SkIntToScalar(glyph->fBounds.fLeft + SK_DistanceFieldInset);
    SkScalar dy = SkIntToScalar(glyph->fBounds.fTop + SK_DistanceFieldInset);
    SkScalar width = SkIntToScalar(glyph->fBounds.width() - 2*SK_DistanceFieldInset);
    SkScalar height = SkIntToScalar(glyph->fBounds.height() - 2*SK_DistanceFieldInset);

    SkScalar scale = fTextRatio;
    dx *= scale;
    dy *= scale;
    sx += dx;
    sy += dy;
    width *= scale;
    height *= scale;

    SkFixed tx = SkIntToFixed(glyph->fAtlasLocation.fX + SK_DistanceFieldInset);
    SkFixed ty = SkIntToFixed(glyph->fAtlasLocation.fY + SK_DistanceFieldInset);
    SkFixed tw = SkIntToFixed(glyph->fBounds.width() - 2*SK_DistanceFieldInset);
    SkFixed th = SkIntToFixed(glyph->fBounds.height() - 2*SK_DistanceFieldInset);

    SkRect r;
    r.fLeft = sx;
    r.fTop = sy;
    r.fRight = sx + width;
    r.fBottom = sy + height;

    fVertexBounds.joinNonEmptyArg(r);

    size_t vertSize = fUseLCDText ? (2 * sizeof(SkPoint))
                                  : (2 * sizeof(SkPoint) + sizeof(GrColor));
    
    SkASSERT(vertSize == fDrawTarget->getDrawState().getVertexStride());

    SkPoint* positions = reinterpret_cast<SkPoint*>(
                               reinterpret_cast<intptr_t>(fVertices) + vertSize * fCurrVertex);
    positions->setRectFan(r.fLeft, r.fTop, r.fRight, r.fBottom, vertSize);

    // The texture coords are last in both the with and without color vertex layouts.
    SkPoint* textureCoords = reinterpret_cast<SkPoint*>(
                               reinterpret_cast<intptr_t>(positions) + vertSize  - sizeof(SkPoint));
    textureCoords->setRectFan(SkFixedToFloat(texture->texturePriv().normalizeFixedX(tx)),
                              SkFixedToFloat(texture->texturePriv().normalizeFixedY(ty)),
                              SkFixedToFloat(texture->texturePriv().normalizeFixedX(tx + tw)),
                              SkFixedToFloat(texture->texturePriv().normalizeFixedY(ty + th)),
                              vertSize);
    if (useColorVerts) {
        if (0xFF == GrColorUnpackA(fPaint.getColor())) {
            fDrawTarget->drawState()->setHint(GrDrawState::kVertexColorsAreOpaque_Hint, true);
        }
        // color comes after position.
        GrColor* colors = reinterpret_cast<GrColor*>(positions + 1);
        for (int i = 0; i < 4; ++i) {
            *colors = fPaint.getColor();
            colors = reinterpret_cast<GrColor*>(reinterpret_cast<intptr_t>(colors) + vertSize);
        }
    }

    fCurrVertex += 4;
}