static int
pjetxl_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{	/* Initialize PaintJet XL for printing */
        gp_fputs("\033E", prn_stream);
        /* The XL has a different vertical origin, who knows why?? */
        return pj_common_print_page(pdev, prn_stream, -360, "\033*rC");
}