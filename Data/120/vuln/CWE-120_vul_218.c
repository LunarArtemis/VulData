bool
GIFInput::read_subimage_data()
{
    GifColorType* colormap = NULL;
    if (m_gif_file->Image.ColorMap) {  // local colormap
        colormap = m_gif_file->Image.ColorMap->Colors;
    } else if (m_gif_file->SColorMap) {  // global colormap
        colormap = m_gif_file->SColorMap->Colors;
    } else {
        errorf("Neither local nor global colormap present.");
        return false;
    }

    if (m_subimage == 0 || m_previous_disposal_method == DISPOSE_BACKGROUND) {
        // make whole canvas transparent
        std::fill(m_canvas.begin(), m_canvas.end(), 0x00);
    }

    // decode scanline index if image is interlaced
    bool interlacing = m_spec.get_int_attribute("gif:Interlacing") != 0;

    // get subimage dimensions and draw it on canvas
    int window_height = m_gif_file->Image.Height;
    int window_width  = m_gif_file->Image.Width;
    int window_top    = m_gif_file->Image.Top;
    int window_left   = m_gif_file->Image.Left;
    std::unique_ptr<unsigned char[]> fscanline(new unsigned char[window_width]);
    for (int wy = 0; wy < window_height; wy++) {
        if (DGifGetLine(m_gif_file, &fscanline[0], window_width) == GIF_ERROR) {
            report_last_error();
            return false;
        }
        int y = window_top
                + (interlacing ? decode_line_number(wy, window_height) : wy);
        if (0 <= y && y < m_spec.height) {
            for (int wx = 0; wx < window_width; wx++) {
                int x   = window_left + wx;
                int idx = m_spec.nchannels * (y * m_spec.width + x);
                if (0 <= x && x < m_spec.width
                    && fscanline[wx] != m_transparent_color) {
                    m_canvas[idx]     = colormap[fscanline[wx]].Red;
                    m_canvas[idx + 1] = colormap[fscanline[wx]].Green;
                    m_canvas[idx + 2] = colormap[fscanline[wx]].Blue;
                    m_canvas[idx + 3] = 0xff;
                }
            }
        }
    }

    return true;
}