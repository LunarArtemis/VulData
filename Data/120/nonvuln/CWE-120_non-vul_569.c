void UnicodeBlockCanvas::Send(int x, int dy, const Framebuffer &framebuffer,
                              SeqType seq_type, Duration end_of_frame) {
    const int width  = framebuffer.width();
    const int height = framebuffer.height();
    OutBuffer out_buffer(RequestBuffers(width, height), 0);
    char *pos = out_buffer.data;

    if (dy < 0) MoveCursorDY(cell_height_for_pixels(dy));

    pos = AppendPrefixToBuffer(pos);

    if (use_quarter_blocks_) x /= 2;  // That is in character cell units.

    const char *before_image_emission = pos;

    const rgba_t *const pixels = framebuffer.begin();
    const rgba_t *top_row, *bottom_row;

    // If we just got requested to move back where we started the last image,
    // we just need to emit pixels that changed.
    prev_content_it_           = backing_buffer_;
    const bool emit_difference = (x == last_x_indent_) &&
                                 (last_framebuffer_height_ > 0) &&
                                 abs(dy) == last_framebuffer_height_;

    // We are always writing two lines at once with one character, which
    // requires to leave an empty line if the height of the framebuffer is odd.
    // We want to make sure that this empty line is written in natural terminal
    // background color to match the chosen terminal color.
    // Depending on if we use the upper or lower half block character to show
    // pixels, we might need to shift displaying by one pixel to make sure
    // the empty line matches up with the background part of that character.
    // This it the row_offset we calculate here.
    const bool needs_empty_line   = (height % 2 != 0);
    const bool top_optional_blank = !use_upper_half_block_;
    const int row_offset = (needs_empty_line && top_optional_blank) ? -1 : 0;

    int y_skip = 0;
    for (int y = 0; y < height; y += 2) {
        const int row = y + row_offset;
        top_row       = row < 0 ? empty_line_ : &pixels[width * row];
        bottom_row =
            (row + 1) >= height ? empty_line_ : &pixels[width * (row + 1)];

        if (use_256_color_) {
            if (use_quarter_blocks_) {
                pos = AppendDoubleRow<2, 8>(pos, x, width, top_row, bottom_row,
                                            emit_difference, &y_skip);
            }
            else {
                pos = AppendDoubleRow<1, 8>(pos, x, width, top_row, bottom_row,
                                            emit_difference, &y_skip);
            }
        }
        else {
            if (use_quarter_blocks_) {
                pos = AppendDoubleRow<2, 24>(pos, x, width, top_row, bottom_row,
                                             emit_difference, &y_skip);
            }
            else {
                pos = AppendDoubleRow<1, 24>(pos, x, width, top_row, bottom_row,
                                             emit_difference, &y_skip);
            }
        }
    }
    last_framebuffer_height_ = height;
    last_x_indent_           = x;
    if (before_image_emission == pos) {
        // Don't even emit cursor up/dn jump, keep buffer size zero.
        write_sequencer_->WriteBuffer(std::move(out_buffer), seq_type,
                                      end_of_frame);
        return;
    }

    if (y_skip) {
        pos += sprintf(pos, SCREEN_CURSOR_DN_FORMAT, y_skip);
    }
    out_buffer.size = (size_t)(pos - out_buffer.data);
    write_sequencer_->WriteBuffer(std::move(out_buffer), seq_type,
                                  end_of_frame);
}