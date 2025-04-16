static int
pc_write_mono_palette(gx_device * dev, uint max_index, gp_file * file)
{
    uint i, c;
    gx_color_value rgb[3];

    for (i = 0; i < max_index; i++) {
        rgb[0] = rgb[1] = rgb[2] = i << 8;
        for (c = 0; c < 3; c++) {
            byte b = gx_color_value_to_byte(rgb[c]);

            gp_fputc(b, file);
        }
    }
    return 0;
}