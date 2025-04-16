char *UnicodeBlockCanvas::RequestBuffers(int width, int height) {
    // Pixels will be variable size depending on if we need to change colors
    // between two adjacent pixels. This is the maximum size they can be.
    static const int max_pixel_size =
        strlen("\033[")                               //
        + PIXEL_SET_COLOR_LEN + ESCAPE_COLOR_MAX_LEN  //
        + 1                                           /* ; */
        + PIXEL_SET_COLOR_LEN + ESCAPE_COLOR_MAX_LEN  //
        + 1                                           /* m */
        + PIXEL_BLOCK_CHARACTER_LEN;
    // Few extra space for number printed in the format.
    static const int opt_cursor_up    = strlen(SCREEN_CURSOR_UP_FORMAT) + 3;
    static const int opt_cursor_right = strlen(SCREEN_CURSOR_RIGHT_FORMAT) + 3;
    const int vertical_characters = (height + 1) / 2;  // two pixels, one glyph
    const size_t content_size =
        opt_cursor_up  // Jump up
        +
        vertical_characters * (opt_cursor_right            // Horizontal jump
                               + width * max_pixel_size    // pixels in one row
                               + SCREEN_END_OF_LINE_LEN);  // Finishing a line.

    // Depending on even/odd situation, we might need one extra row.
    const size_t new_backing = width * (height + 1) * sizeof(rgba_t);
    if (new_backing > backing_buffer_size_) {
        backing_buffer_      = (rgba_t *)realloc(backing_buffer_, new_backing);
        backing_buffer_size_ = new_backing;
    }

    const size_t new_empty = width * sizeof(rgba_t);
    if (new_empty > empty_line_size_) {
        empty_line_      = (rgba_t *)realloc(empty_line_, new_empty);
        empty_line_size_ = new_empty;
        memset(empty_line_, 0x00, empty_line_size_);
    }
    return new char[content_size];
}