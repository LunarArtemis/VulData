void* Zone::New(size_t size) {
  // Round up the requested size to fit the alignment.
  size = RoundUp(size, kAlignment);

  // If the allocation size is divisible by 8 then we return an 8-byte aligned
  // address.
  if (kPointerSize == 4 && kAlignment == 4) {
    position_ += ((~size) & 4) & (reinterpret_cast<intptr_t>(position_) & 4);
  } else {
    DCHECK(kAlignment >= kPointerSize);
  }

  // Check if the requested size is available without expanding.
  Address result = position_;

  const size_t size_with_redzone = size + kASanRedzoneBytes;
  if (limit_ < position_ + size_with_redzone) {
    result = NewExpand(size_with_redzone);
  } else {
    position_ += size_with_redzone;
  }

  Address redzone_position = result + size;
  DCHECK(redzone_position + kASanRedzoneBytes == position_);
  ASAN_POISON_MEMORY_REGION(redzone_position, kASanRedzoneBytes);

  // Check that the result has the proper alignment and return it.
  DCHECK(IsAddressAligned(result, kAlignment, 0));
  allocation_size_ += size;
  return reinterpret_cast<void*>(result);
}