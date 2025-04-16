void pdf_print::init_hv()
{
    dvi_h = 0;
    dvi_v = pdf_top_of_page ;
    pdf_command(MOVE, dvi_h, -dvi_v, 0, 0);
}