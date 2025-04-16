static void strided_copy(void* dest, const void* src, int width, int height,
                         int stride)
{
  if (width == stride) {
    memcpy(dest, src, width * height);
  }
  else {
    const uint8_t* _src = static_cast<const uint8_t*>(src);
    uint8_t* _dest = static_cast<uint8_t*>(dest);
    for (int y = 0; y < height; y++, _dest += width, _src += stride) {
      memcpy(_dest, _src, stride);
    }
  }
}