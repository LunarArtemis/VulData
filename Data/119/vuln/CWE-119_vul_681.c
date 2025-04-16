void PartialCircularBuffer::DoWrite(void* dest, const void* src, uint32 num) {
  memcpy(dest, src, num);
  position_ += num;
  buffer_data_->total_written =
      std::min(buffer_data_->total_written + num, data_size_);
  buffer_data_->end_position = position_;
}