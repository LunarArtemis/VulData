int InMemoryUrlProtocol::Read(int size, uint8* data) {
  int available_bytes = size_ - position_;
  if (size > available_bytes)
    size = available_bytes;

  memcpy(data, data_ + position_, size);
  position_ += size;
  return size;
}