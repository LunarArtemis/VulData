bool WebGraphicsContext3DImpl::getActiveAttrib(
    WebGLId program, WGC3Duint index, ActiveInfo& info) {
  if (!program) {
    synthesizeGLError(GL_INVALID_VALUE);
    return false;
  }
  GLint max_name_length = -1;
  gl_->GetProgramiv(
      program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_length);
  if (max_name_length < 0)
    return false;
  scoped_ptr<GLchar[]> name(new GLchar[max_name_length]);
  if (!name) {
    synthesizeGLError(GL_OUT_OF_MEMORY);
    return false;
  }
  GLsizei length = 0;
  GLint size = -1;
  GLenum type = 0;
  gl_->GetActiveAttrib(
      program, index, max_name_length, &length, &size, &type, name.get());
  if (size < 0) {
    return false;
  }
  info.name = blink::WebString::fromUTF8(name.get(), length);
  info.type = type;
  info.size = size;
  return true;
}