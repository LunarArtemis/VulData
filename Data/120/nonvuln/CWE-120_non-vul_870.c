static int
devn_write_pcx_file(gx_device_printer * pdev, char * filename, int ncomp,
                            int bpc, int linesize)
{
    pcx_header header;
    int code;
    bool planar;
    char *outname = (char *)gs_alloc_bytes(pdev->memory, gp_file_name_sizeof, "devn_write_pcx_file(outname)");
    gp_file * in = NULL;
    gp_file * out = NULL;
    int depth = bpc_to_depth(ncomp, bpc);

    if (outname == NULL) {
        code = gs_note_error(gs_error_VMerror);
        goto done;
    }

    code = gs_add_control_path(pdev->memory, gs_permit_file_reading, filename);
    if (code < 0)
        goto done;

    in = gp_fopen(pdev->memory, filename, "rb");
    if (!in) {
        code = gs_note_error(gs_error_invalidfileaccess);
        goto done;
    }
    gs_snprintf(outname, gp_file_name_sizeof, "%s.pcx", filename);
    code = gs_add_control_path(pdev->memory, gs_permit_file_writing, outname);
    if (code < 0)
        goto done;
    out = gp_fopen(pdev->memory, outname, "wb");
    if (!out) {
        code = gs_note_error(gs_error_invalidfileaccess);
        goto done;
    }

    if (ncomp == 4 && bpc == 8) {
        ncomp = 3;		/* we will convert 32-bit to 24-bit RGB */
    }
    planar = devn_setup_pcx_header(pdev, &header, ncomp, bpc);
    code = devn_pcx_write_page(pdev, in, linesize, out, &header, planar, depth);
    if (code >= 0)
        code = devn_finish_pcx_file(pdev, out, &header, ncomp, bpc);

done:
    (void)gs_remove_control_path(pdev->memory, gs_permit_file_reading, filename);
    (void)gs_remove_control_path(pdev->memory, gs_permit_file_writing, outname);
    if (in)
      gp_fclose(in);
    if (out)
      gp_fclose(out);

    if (outname)
      gs_free_object(pdev->memory, outname, "spotcmyk_print_page(outname)");

    return code;
}