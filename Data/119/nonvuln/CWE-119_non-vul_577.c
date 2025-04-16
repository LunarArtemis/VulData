void pdf_print::do_tie_reversed(double length) 
{ 
    pdf_command ( PRTIE, inch_to_dvi(length), 0, 0, 0);
}