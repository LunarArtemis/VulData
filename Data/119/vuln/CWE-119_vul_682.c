void PartialCircularBuffer::Write(const void* buffer, uint32 buffer_size) {
  DCHECK(buffer_data_);
  uint32 position_before_write = position_;

  uint32 to_eof = data_size_ - position_;
  uint32 to_write = std::min(buffer_size, to_eof);
  DoWrite(buffer_data_->data + position_, buffer, to_write);
  if (position_ >= data_size_) {
    DCHECK_EQ(position_, data_size_);
    position_ = buffer_data_->wrap_position;
  }

  if (to_write < buffer_size) {
    uint32 remainder_to_write = buffer_size - to_write;
    DCHECK_LT(position_, position_before_write);
    DCHECK_LE(position_ + remainder_to_write, position_before_write);
    DoWrite(buffer_data_->data + position_,
            reinterpret_cast<const uint8*>(buffer) + to_write,
            remainder_to_write);
  }
}