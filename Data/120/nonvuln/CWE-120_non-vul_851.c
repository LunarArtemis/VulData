static int
spotcmyk_print_page(gx_device_printer * pdev, gp_file * prn_stream)
{
    int line_size = gdev_mem_bytes_per_scan_line((gx_device *) pdev);
    byte *in = gs_alloc_bytes(pdev->memory, line_size, "spotcmyk_print_page(in)");
    byte *buf = gs_alloc_bytes(pdev->memory, line_size + 3, "spotcmyk_print_page(buf)");
    const gx_devn_prn_device * pdevn = (gx_devn_prn_device *) pdev;
    uint npcmcolors = pdevn->devn_params.num_std_colorant_names;
    uchar ncomp = pdevn->color_info.num_components;
    int depth = pdevn->color_info.depth;
    int nspot = pdevn->devn_params.separations.num_separations;
    int bpc = pdevn->devn_params.bitspercomponent;
    int lnum = 0, bottom = pdev->height;
    int width = pdev->width;
    gp_file * spot_file[GX_DEVICE_COLOR_MAX_COMPONENTS] = {0};
    uint i;
    int code = 0;
    int first_bit;
    int pcmlinelength = 0; /* Initialize against indeterminizm in case of pdev->height == 0. */
    int linelength[GX_DEVICE_COLOR_MAX_COMPONENTS];
    byte *data;
    char *spotname = (char *)gs_alloc_bytes(pdev->memory, gp_file_name_sizeof, "spotcmyk_print_page(spotname)");

    if (in == NULL || buf == NULL || spotname == NULL) {
        code = gs_note_error(gs_error_VMerror);
        goto prn_done;
    }
    /*
     * Check if the SeparationOrder list has changed the order of the process
     * color model colorants. If so then we will treat all colorants as if they
     * are spot colors.
     */
    for (i = 0; i < npcmcolors; i++)
        if (pdevn->devn_params.separation_order_map[i] != i)
            break;
    if (i < npcmcolors || ncomp < npcmcolors) {
        nspot = ncomp;
        npcmcolors = 0;
    }

    /* Open the output files for the spot colors */
    for(i = 0; i < nspot; i++) {
        gs_snprintf(spotname, gp_file_name_sizeof, "%ss%d", pdevn->fname, i);
        code = gs_add_control_path(pdev->memory, gs_permit_file_writing, spotname);
        if (code < 0)
            goto prn_done;
        spot_file[i] = gp_fopen(pdev->memory, spotname, "wb");
        (void)gs_remove_control_path(pdev->memory, gs_permit_file_writing, spotname);
        if (spot_file[i] == NULL) {
            code = gs_note_error(gs_error_VMerror);
            goto prn_done;
        }
    }

    /* Now create the output bit image files */
    for (; lnum < bottom; ++lnum) {
        code = gdev_prn_get_bits(pdev, lnum, in, &data);
        if (code < 0)
            goto prn_done;
        /* Now put the pcm data into the output file */
        if (npcmcolors) {
            first_bit = bpc * (ncomp - npcmcolors);
            pcmlinelength = repack_data(data, buf, depth, first_bit, bpc * npcmcolors, width);
            gp_fwrite(buf, 1, pcmlinelength, prn_stream);
        }
        /* Put spot color data into the output files */
        for (i = 0; i < nspot; i++) {
            first_bit = bpc * (nspot - 1 - i);
            linelength[i] = repack_data(data, buf, depth, first_bit, bpc, width);
            gp_fwrite(buf, 1, linelength[i], spot_file[i]);
        }
    }

    /* Close the bit image files */
    for(i = 0; i < nspot; i++) {
        gp_fclose(spot_file[i]);
        spot_file[i] = NULL;
    }

    /* Now convert the bit image files into PCX files */
    if (npcmcolors) {
        code = devn_write_pcx_file(pdev, (char *) &pdevn->fname,
                                npcmcolors, bpc, pcmlinelength);
        if (code < 0)
            goto prn_done;
    }
    for(i = 0; i < nspot; i++) {
        gs_snprintf(spotname, gp_file_name_sizeof, "%ss%d", pdevn->fname, i);
        code = devn_write_pcx_file(pdev, spotname, 1, bpc, linelength[i]);
        if (code < 0)
            goto prn_done;
    }

    /* Clean up and exit */
  prn_done:
    for(i = 0; i < nspot; i++) {
        if (spot_file[i] != NULL)
            gp_fclose(spot_file[i]);
    }
    if (in != NULL)
        gs_free_object(pdev->memory, in, "spotcmyk_print_page(in)");
    if (buf != NULL)
        gs_free_object(pdev->memory, buf, "spotcmyk_print_page(buf)");
    if (spotname != NULL)
        gs_free_object(pdev->memory, spotname, "spotcmyk_print_page(spotname)");
    return code;
}