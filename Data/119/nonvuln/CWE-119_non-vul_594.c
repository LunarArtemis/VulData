void pdf_print::do_half_tie(double length) 
{ 
    pdf_command ( PHTIE, inch_to_dvi(length), 0, 0, 0);
}