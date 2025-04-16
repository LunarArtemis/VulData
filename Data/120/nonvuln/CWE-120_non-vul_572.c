UnicodeBlockCanvas::~UnicodeBlockCanvas() {
    free(backing_buffer_);
    free(empty_line_);
}