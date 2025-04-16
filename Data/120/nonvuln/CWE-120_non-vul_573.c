UnicodeBlockCanvas::UnicodeBlockCanvas(BufferedWriteSequencer *ws,
                                       bool use_quarter,
                                       bool use_upper_half_block,
                                       bool use_256_color)
    : TerminalCanvas(ws),
      use_quarter_blocks_(use_quarter),
      use_upper_half_block_(use_upper_half_block),
      use_256_color_(use_256_color) {}