static int
paintjet_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{	/* ends raster graphics to set raster graphics resolution */
        gp_fputs("\033*rB", prn_stream);
        return pj_common_print_page(pdev, prn_stream, 0, "\033*r0B\014");
}