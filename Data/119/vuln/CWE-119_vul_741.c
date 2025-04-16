SkFlattenable* SkPictureShader::CreateProc(SkReadBuffer& buffer) {
    SkMatrix lm;
    buffer.readMatrix(&lm);
    TileMode mx = (TileMode)buffer.read32();
    TileMode my = (TileMode)buffer.read32();
    SkRect tile;
    buffer.readRect(&tile);
    SkAutoTUnref<SkPicture> picture(SkPicture::CreateFromBuffer(buffer));
    return SkPictureShader::Create(picture, mx, my, &lm, &tile);
}