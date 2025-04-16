void RendererClipboardWriteContext::WriteBitmapFromPixels(
    ui::Clipboard::ObjectMap* objects,
    const void* pixels,
    const gfx::Size& size) {
  // Do not try to write a bitmap more than once
  if (shared_buf_)
    return;

  uint32 buf_size = 4 * size.width() * size.height();

  // Allocate a shared memory buffer to hold the bitmap bits.
  shared_buf_.reset(ChildThread::current()->AllocateSharedMemory(buf_size));
  if (!shared_buf_)
    return;

  // Copy the bits into shared memory
  DCHECK(shared_buf_->memory());
  memcpy(shared_buf_->memory(), pixels, buf_size);
  shared_buf_->Unmap();

  ui::Clipboard::ObjectMapParam size_param;
  const char* size_data = reinterpret_cast<const char*>(&size);
  for (size_t i = 0; i < sizeof(gfx::Size); ++i)
    size_param.push_back(size_data[i]);

  ui::Clipboard::ObjectMapParams params;

  // The first parameter is replaced on the receiving end with a pointer to
  // a shared memory object containing the bitmap. We reserve space for it here.
  ui::Clipboard::ObjectMapParam place_holder_param;
  params.push_back(place_holder_param);
  params.push_back(size_param);
  (*objects)[ui::Clipboard::CBF_SMBITMAP] = params;
}