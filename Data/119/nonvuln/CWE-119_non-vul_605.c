void pdf_print::do_rtie(int bloc, int eloc) 
{ 
    pdf_command ( PTIE, (save_h[eloc] - save_h[bloc]), 0, 0, 0);
}