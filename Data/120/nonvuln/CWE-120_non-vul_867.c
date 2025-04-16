static bool
devn_setup_pcx_header(gx_device_printer * pdev, pcx_header * phdr, int num_planes, int bits_per_plane)
{
    bool planar = true; /* Invalid cases could cause an indeterminizm. */

    *phdr = pcx_header_prototype;
    phdr->bpp = bits_per_plane;
    phdr->nplanes = num_planes;

    switch (num_planes) {
        case 1:
            switch (bits_per_plane) {
                case 1:
                        phdr->version = version_2_8_with_palette;
                        assign_ushort(phdr->palinfo, palinfo_gray);
                        memcpy((byte *) phdr->palette, "\000\000\000\377\377\377", 6);
                        planar = false;
                        break;
                case 2:                         /* Not defined */
                        break;
                case 4:
                        phdr->version = version_2_8_with_palette;
                        memcpy((byte *) phdr->palette, pcx_ega_palette, sizeof(pcx_ega_palette));
                        planar = true;
                        break;
                case 5:                         /* Not defined */
                        break;
                case 8:
                        phdr->version = version_3_0;
                        assign_ushort(phdr->palinfo, palinfo_gray);
                        planar = false;
                        break;
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
                case 8:
                        phdr->version = version_3_0;
                        assign_ushort(phdr->palinfo, palinfo_color);
                        planar = true;
                        break;
                case 16:                        /* Not defined */
                        break;
            }
            break;
        case 4:
            switch (bits_per_plane) {
                case 1:
                        phdr->version = 2;
                        memcpy((byte *) phdr->palette, pcx_cmyk_palette,
                                sizeof(pcx_cmyk_palette));
                        planar = false;
                        phdr->bpp = 4;
                        phdr->nplanes = 1;
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
    return planar;
}