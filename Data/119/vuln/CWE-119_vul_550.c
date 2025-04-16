void CanvasCaptureHandler::CreateNewFrame(const SkImage* image) {
  DVLOG(4) << __FUNCTION__;
  DCHECK(main_render_thread_checker_.CalledOnValidThread());
  DCHECK(image);

  const gfx::Size size(image->width(), image->height());
  if (size != last_size) {
    temp_data_.resize(
        media::VideoFrame::AllocationSize(media::PIXEL_FORMAT_ARGB, size));
    row_bytes_ =
        media::VideoFrame::RowBytes(0, media::PIXEL_FORMAT_ARGB, size.width());
    image_info_ =
        SkImageInfo::Make(size.width(), size.height(), kBGRA_8888_SkColorType,
                          kUnpremul_SkAlphaType);
    last_size = size;
  }

  if(!image->readPixels(image_info_, &temp_data_[0], row_bytes_, 0, 0)) {
    DLOG(ERROR) << "Couldn't read SkImage pixels";
    return;
  }

  const bool isOpaque = image->isOpaque();
  const base::TimeTicks timestamp = base::TimeTicks::Now();
  scoped_refptr<media::VideoFrame> video_frame = frame_pool_.CreateFrame(
      isOpaque ? media::PIXEL_FORMAT_I420 : media::PIXEL_FORMAT_YV12A, size,
      gfx::Rect(size), size, timestamp - base::TimeTicks());
  DCHECK(video_frame);

  libyuv::ARGBToI420(temp_data_.data(), row_bytes_,
                     video_frame->data(media::VideoFrame::kYPlane),
                     video_frame->stride(media::VideoFrame::kYPlane),
                     video_frame->data(media::VideoFrame::kUPlane),
                     video_frame->stride(media::VideoFrame::kUPlane),
                     video_frame->data(media::VideoFrame::kVPlane),
                     video_frame->stride(media::VideoFrame::kVPlane),
                     size.width(), size.height());
  if (!isOpaque) {
    // TODO(emircan): Use https://code.google.com/p/libyuv/issues/detail?id=572
    // when it becomes available.
    CopyAlphaChannelIntoVideoFrame(temp_data_.data(), video_frame);
  }

  last_frame_ = video_frame;
  io_task_runner_->PostTask(
      FROM_HERE,
      base::Bind(&CanvasCaptureHandler::CanvasCaptureHandlerDelegate::
                     SendNewFrameOnIOThread,
                 delegate_->GetWeakPtrForIOThread(), video_frame, timestamp));
}