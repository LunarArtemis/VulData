static int
devn_finish_pcx_file(gx_device_printer * pdev, gp_file * file, pcx_header * header, int num_planes, int bits_per_plane)
{
    switch (num_planes) {
        case 1:
            switch (bits_per_plane) {
                case 1:                         /* Do nothing */
                        break;
                case 2:                         /* Not defined */
                        break;
                case 4:                         /* Do nothing */
                        break;
                case 5:                         /* Not defined */
                        break;
                case 8:
                        gp_fputc(0x0c, file);
                        return pc_write_mono_palette((gx_device *) pdev, 256, file);
                case 16:                        /* Not defined */
                        break;
            }
            break;
        case 2:
            switch (bits_per_plane) {
                case 1:                         /* Not defined */
                        break;
                case 2:                         /* Not defined */
                        break;
                case 4:                         /* Not defined */
                        break;
                case 5:                         /* Not defined */
                        break;
                case 8:                         /* Not defined */
                        break;
                case 16:                        /* Not defined */
                        break;
            }
            break;
        case 3:
            switch (bits_per_plane) {
                case 1:                         /* Not defined */
                        break;
                case 2:                         /* Not defined */
                        break;
                case 4:                         /* Not defined */
                        break;
                case 5:                         /* Not defined */
                        break;
                case 8:                         /* Do nothing */
                        break;
                case 16:                        /* Not defined */
                        break;
            }
            break;
        case 4:
            switch (bits_per_plane) {
                case 1:                         /* Do nothing */
                        break;
                case 2:                         /* Not defined */
                        break;
                case 4:                         /* Not defined */
                        break;
                case 5:                         /* Not defined */
                        break;
                case 8:                         /* Not defined */
                        break;
                case 16:                        /* Not defined */
                        break;
            }
            break;
    }
    return 0;
}