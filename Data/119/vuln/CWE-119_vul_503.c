Address Zone::NewExpand(size_t size) {
  // Make sure the requested size is already properly aligned and that
  // there isn't enough room in the Zone to satisfy the request.
  DCHECK_EQ(size, RoundDown(size, kAlignment));
  DCHECK_LT(limit_, position_ + size);

  // Compute the new segment size. We use a 'high water mark'
  // strategy, where we increase the segment size every time we expand
  // except that we employ a maximum segment size when we delete. This
  // is to avoid excessive malloc() and free() overhead.
  Segment* head = segment_head_;
  const size_t old_size = (head == nullptr) ? 0 : head->size();
  static const size_t kSegmentOverhead = sizeof(Segment) + kAlignment;
  const size_t new_size_no_overhead = size + (old_size << 1);
  size_t new_size = kSegmentOverhead + new_size_no_overhead;
  const size_t min_new_size = kSegmentOverhead + size;
  // Guard against integer overflow.
  if (new_size_no_overhead < size || new_size < kSegmentOverhead) {
    V8::FatalProcessOutOfMemory("Zone");
    return nullptr;
  }
  if (new_size < kMinimumSegmentSize) {
    new_size = kMinimumSegmentSize;
  } else if (new_size > kMaximumSegmentSize) {
    // Limit the size of new segments to avoid growing the segment size
    // exponentially, thus putting pressure on contiguous virtual address space.
    // All the while making sure to allocate a segment large enough to hold the
    // requested size.
    new_size = Max(min_new_size, kMaximumSegmentSize);
  }
  if (new_size > INT_MAX) {
    V8::FatalProcessOutOfMemory("Zone");
    return nullptr;
  }
  Segment* segment = NewSegment(new_size);
  if (segment == nullptr) {
    V8::FatalProcessOutOfMemory("Zone");
    return nullptr;
  }

  // Recompute 'top' and 'limit' based on the new segment.
  Address result = RoundUp(segment->start(), kAlignment);
  position_ = result + size;
  // Check for address overflow.
  // (Should not happen since the segment is guaranteed to accomodate
  // size bytes + header and alignment padding)
  DCHECK(reinterpret_cast<uintptr_t>(position_) >=
         reinterpret_cast<uintptr_t>(result));
  limit_ = segment->end();
  DCHECK(position_ <= limit_);
  return result;
}