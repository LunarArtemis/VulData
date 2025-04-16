static int
lj250_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{	gp_fputs("\033%8", prn_stream);	/* Enter PCL emulation mode */
        /* ends raster graphics to set raster graphics resolution */
        gp_fputs("\033*rB", prn_stream);
        /* Exit PCL emulation mode after printing */
        return pj_common_print_page(pdev, prn_stream, 0, "\033*r0B\014\033%@");
}