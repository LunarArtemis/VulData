bool graphicsGetFromVar(JsGraphics *gfx, JsVar *parent) {
  gfx->graphicsVar = parent;
  JsVar *data = jsvObjectGetChild(parent, JS_HIDDEN_CHAR_STR"gfx", 0);
  assert(data);
  if (data) {
    jsvGetString(data, (char*)&gfx->data, sizeof(JsGraphicsData)+1/*trailing zero*/);
    jsvUnLock(data);
    gfx->setPixel = graphicsFallbackSetPixel;
    gfx->getPixel = graphicsFallbackGetPixel;
    gfx->fillRect = graphicsFallbackFillRect;
    gfx->scroll = graphicsFallbackScroll;
#ifdef USE_LCD_SDL
    if (gfx->data.type == JSGRAPHICSTYPE_SDL) {
      lcdSetCallbacks_SDL(gfx);
    } else
#endif
#ifdef USE_LCD_FSMC
    if (gfx->data.type == JSGRAPHICSTYPE_FSMC) {
      lcdSetCallbacks_FSMC(gfx);
    } else
#endif
    if (gfx->data.type == JSGRAPHICSTYPE_ARRAYBUFFER) {
      lcdSetCallbacks_ArrayBuffer(gfx);
#ifndef SAVE_ON_FLASH
    } else if (gfx->data.type == JSGRAPHICSTYPE_JS) {
      lcdSetCallbacks_JS(gfx);
#endif
#ifdef USE_LCD_SPI
    } else if (gfx->data.type == JSGRAPHICSTYPE_SPILCD) {
      lcdSetCallbacks_SPILCD(gfx);
#endif
#ifdef USE_LCD_ST7789_8BIT
    } else if (gfx->data.type == JSGRAPHICSTYPE_ST7789_8BIT) {
      lcdST7789_setCallbacks(gfx);
#endif
    } else {
      jsExceptionHere(JSET_INTERNALERROR, "Unknown graphics type\n");
      assert(0);
    }

    return true;
  } else
    return false;
}