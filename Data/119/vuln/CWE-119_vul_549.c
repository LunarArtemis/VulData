static void CopyAlphaChannelIntoVideoFrame(
    const uint8_t* const source,
    const scoped_refptr<media::VideoFrame>& dest_frame) {
  const gfx::Size& size = dest_frame->coded_size();
  const int stride = dest_frame->stride(media::VideoFrame::kAPlane);

  if (stride == size.width()) {
    for (int p = 0; p < size.GetArea(); ++p)
      dest_frame->data(media::VideoFrame::kAPlane)[p] = source[p * 4 + 3];
    return;
  }

  // Copy apha values one-by-one if the destination stride != source width.
  for (int h = 0; h < size.height(); ++h) {
    const uint8_t* const src_ptr = &source[4 * h * size.width()];
    uint8_t* dest_ptr =
        &dest_frame->data(media::VideoFrame::kAPlane)[h * stride];
    for (int pixel_index = 0; pixel_index < 4 * size.width(); pixel_index += 4)
      *(dest_ptr++) = src_ptr[pixel_index + 3];
  }
}