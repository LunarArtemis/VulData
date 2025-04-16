void pdf_print::do_half_tie_reversed(double length) 
{ 
    pdf_command ( PHRTIE, inch_to_dvi(length), 0, 0, 0);
}