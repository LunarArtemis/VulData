void pdf_print::p_moveto(const int hor, const int ver)
{
//    printf("in moveto %d %d\n", hor, ver);
    pdf_command(MOVE, hor, -ver, 0, 0);
    dvi_h = hor;	
    dvi_v = ver;	
}