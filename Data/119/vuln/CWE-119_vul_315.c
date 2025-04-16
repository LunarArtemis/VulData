void ResourceProvider::InitializeSoftware() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK_NE(Bitmap, default_resource_type_);

  CleanUpGLIfNeeded();

  default_resource_type_ = Bitmap;
  max_texture_size_ = INT_MAX / 2;
  best_texture_format_ = RGBA_8888;
}