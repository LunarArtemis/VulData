void pdf_print::do_tie(double length) 
{ 
    pdf_command ( PTIE, inch_to_dvi(length), 0, 0, 0);
}