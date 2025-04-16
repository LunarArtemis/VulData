char *UnicodeBlockCanvas::AppendDoubleRow(char *pos, int indent, int width,
                                          const rgba_t *tline,
                                          const rgba_t *bline, bool emit_diff,
                                          int *y_skip) {
    static constexpr char kStartEscape[] = "\033[";
    GlyphPick last                       = {};
    rgba_t last_foreground               = {};
    bool last_fg_unknown                 = true;
    bool last_bg_unknown                 = true;
    int x_skip                           = indent;
    const char *start                    = pos;
    for (int x = 0; x < width;
         x += N, prev_content_it_ += 2 * N, tline += N, bline += N) {
        if (emit_diff && EqualToBacking<N>(tline, bline, prev_content_it_)) {
            ++x_skip;
            continue;
        }

        if (*y_skip) {  // Emit cursor down or newlines, whatever is shorter
            if (*y_skip <= 4) {
                memset(pos, '\n', *y_skip);
                pos += *y_skip;
            }
            else {
                pos += sprintf(pos, SCREEN_CURSOR_DN_FORMAT, *y_skip);
            }
            *y_skip = 0;
        }

        if (x_skip > 0) {
            pos += sprintf(pos, SCREEN_CURSOR_RIGHT_FORMAT, x_skip);
            x_skip = 0;
        }

        const GlyphPick pick = FindBestGlyph<N>(tline, bline);

        bool color_emitted = false;

        // Foreground. Only consider if we're not having background.
        if (pick.block != kBackground &&
            (last_fg_unknown || pick.fg != last_foreground)) {
            // Appending prefix. At this point, it can only be kStartEscape
            pos = str_append(pos, kStartEscape, strlen(kStartEscape));
            pos = str_append(pos, AnsiSetFG<colorbits>(), PIXEL_SET_COLOR_LEN);
            pos = AnsiWriteColor<colorbits>(pos, pick.fg);
            color_emitted   = true;
            last_foreground = pick.fg;
            last_fg_unknown = false;
        }

        // Background
        if (last_bg_unknown || pick.bg != last.bg) {
            if (!color_emitted) {
                pos = str_append(pos, kStartEscape, strlen(kStartEscape));
            }
            if (is_transparent(pick.bg)) {
                // This is best effort and only happens with -b none
                pos = str_append(pos, "49;", 3);  // Reset background color
            }
            else {
                pos = str_append(pos, AnsiSetBG<colorbits>(),
                                 PIXEL_SET_COLOR_LEN);
                pos = AnsiWriteColor<colorbits>(pos, pick.bg);
            }
            color_emitted   = true;
            last_bg_unknown = false;
        }

        if (color_emitted) {
            *(pos - 1) = 'm';  // overwrite semicolon with finish ESC seq.
        }
        if (pick.block == kBackground) {
            *pos++ = ' ';  // Simple background 'block'. One character.
        }
        else {
            pos = str_append(pos, kBlockGlyphs[pick.block],
                             PIXEL_BLOCK_CHARACTER_LEN);
        }
        last = pick;
        StoreBacking<N>(prev_content_it_, tline, bline);
    }

    if (pos == start) {  // Nothing emitted for whole line
        (*y_skip)++;
    }
    else {
        pos = str_append(pos, SCREEN_END_OF_LINE, SCREEN_END_OF_LINE_LEN);
    }

    return pos;
}